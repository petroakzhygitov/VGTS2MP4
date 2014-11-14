#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include "compat.h"
#include "printing_fns.h"
#include "pes_defns.h"
#include "pes_fns.h"
#include "misc_fns.h"
#include "ts_fns.h"
#include "pidint_fns.h"

static int write_packet(uint32_t pid,
        FILE *output,
        int payload_unit_start_indicator,
        int *need_packet_start,
        int *pes_packet_len,
        int *got_pes_packet_len,
        byte *payload,
        int payload_len,
        int *count,
        int *extracted,
        int verbose)
{
  byte  *data;
  int    data_len;
  size_t written;

  if (verbose)
  {
    fprint_msg("%4d: TS Packet PID %04x",*count,pid);
    if (payload_unit_start_indicator)
      print_msg(" (start)");
    else if (*need_packet_start)
      print_msg(" <ignored>");
    print_msg("\n");
  }


  if (payload_unit_start_indicator)
  {
    // It's the start of a PES packet, so we need to drop the header
    int offset;

    if (*need_packet_start)
      *need_packet_start = FALSE;

    *pes_packet_len = (payload[4] << 8) | payload[5];
    if (verbose) fprint_msg("PES packet length %d\n",*pes_packet_len);
    *got_pes_packet_len = (*pes_packet_len > 0);

    if (IS_H222_PES(payload))
    {
      // It's H.222.0 - payload[8] is the PES_header_data_length,
      // so our ES data starts that many bytes after that field
      offset = payload[8] + 9;
    }
    else
    {
      // We assume it's MPEG-1
      offset = calc_mpeg1_pes_offset(payload,payload_len);
    }
    data = &payload[offset];
    data_len = payload_len-offset;
    if (verbose) print_data(TRUE,"data",data,data_len,1000);
  }
  else
  {
    // If we haven't *started* a packet, we can't use this,
    // since it will just look like random bytes when written out.
    if (*need_packet_start)
    {
      return 0;
    }

    data = payload;
    data_len = payload_len;
    if (verbose) print_data(TRUE,"Data",payload,payload_len,1000);

    if (*got_pes_packet_len)
    {
      // Try not to write more data than the PES packet declares
      if (data_len > *pes_packet_len)
      {
        data_len = *pes_packet_len;
        if (verbose) print_data(TRUE,"Reduced data",data,data_len,1000);
        *pes_packet_len = 0;
      }
      else
        *pes_packet_len -= data_len;
    }
  }
  if (data_len > 0)
  {
    // Windows doesn't seem to like writing 0 bytes, so be careful...
    written = fwrite(data,data_len,1,output);
    if (written != 1)
    {
      fprint_err("### Error writing TS packet - units written = %d\n",
              (int)written);
      return 1;
    }
  }
  (*extracted) ++;
  return 0;
}


/*
 * Extract all the TS packets for a nominated PIDs to files.
 *
 * Returns 0 if all went well, 1 if something went wrong.
 */
static int extract_pids_packets(TS_reader_p  tsreader,
        FILE        *output1,
        FILE        *output2,
        uint32_t     pid1_wanted,
        uint32_t     pid2_wanted,
        int          verbose,
        int          quiet)
{
  int    err;
  int    count = 0;
  int    extracted1 = 0;
  int    extracted2 = 0;
  int    pes_packet_len1 = 0;
  int    pes_packet_len2 = 0;
  int    got_pes_packet_len1 = FALSE;
  int    got_pes_packet_len2 = FALSE;
  // It doesn't make sense to start outputting data for our PID until we
  // get the start of a packet
  int    need_packet_start1 = TRUE;
  int    need_packet_start2 = TRUE;

  for (;;)
  {
    uint32_t pid;
    int      payload_unit_start_indicator;
    byte    *adapt, *payload;
    int      adapt_len, payload_len;

    err = get_next_TS_packet(tsreader,&pid,
            &payload_unit_start_indicator,
            &adapt,&adapt_len,&payload,&payload_len);
    if (err == EOF)
      break;
    else if (err)
    {
      print_err("### Error reading TS packet\n");
      return 1;
    }

    count++;

    // If the packet is empty, all we can do is ignore it
    if (payload_len == 0)
      continue;

    if (pid == pid1_wanted)
    {
      err = write_packet(pid, output1, payload_unit_start_indicator, &need_packet_start1, &pes_packet_len1,
              &got_pes_packet_len1, payload, payload_len, &count, &extracted1, verbose);
      if (err)
        return err;
    }
    else if (pid == pid2_wanted)
    {
      err = write_packet(pid, output2, payload_unit_start_indicator, &need_packet_start2, &pes_packet_len2,
              &got_pes_packet_len2, payload, payload_len, &count, &extracted2, verbose);
      if (err)
        return err;
    }
  }

  if (!quiet)
  {
    fprint_msg("Extracted %d of %d TS packet%s for PID=%d\n",
            extracted1, count, (count == 1 ? "" : "s"), pid1_wanted);
    fprint_msg("Extracted %d of %d TS packet%s for PID=%d\n",
            extracted2, count, (count == 1 ? "" : "s"), pid2_wanted);
  }

  // If the user has forgotten to say -pid XX, or -video/-audio,
  // and are piping the output to another program, it can be surprising
  // if there is no data!
  if (quiet && extracted1 == 0)
    fprint_err("### No data extracted for PID %#04x (%d)\n",
            pid1_wanted,pid1_wanted);
  if (quiet && extracted2 == 0)
    fprint_err("### No data extracted for PID %#04x (%d)\n",
            pid2_wanted,pid2_wanted);
  return 0;
}

static int extract_av(int   input,
                      FILE *output1,
                      FILE *output2,
                      int   verbose,
                      int   quiet)
{
  int      err, ii;
  int      total_num_read = 0;
  uint32_t video_pid = 0;
  uint32_t audio_pid = 0;
  TS_reader_p tsreader = NULL;
  pmt_p       pmt = NULL;

  // Turn our file into a TS reader
  err = build_TS_reader(input, &tsreader);
  if (err) return 1;

  // First, find out what program streams we actually have
  for (;;)
  {
    int  num_read;

    err = find_pmt(tsreader, 1, 0, verbose, quiet, &num_read, &pmt);
    if (err == EOF)
    {
      if (!quiet)
        print_msg("No program stream information in the input file\n");
      free_TS_reader(&tsreader);
      free_pmt(&pmt);
      return 0;
    }
    else if (err)
    {
      print_err("### Error finding program stream information\n");
      free_TS_reader(&tsreader);
      free_pmt(&pmt);
      return 1;
    }
    total_num_read += num_read;

    // From that, find first audio and video streams
    // Note that the audio detection will accept either DVB or ADTS Dolby (AC-3) stream types
    for (ii=0; ii < pmt->num_streams; ii++)
    {
      uint32_t pid = pmt->streams[ii].elementary_PID;

      if (!video_pid && IS_VIDEO_STREAM_TYPE(pmt->streams[ii].stream_type))
        video_pid = pid;

      if (!audio_pid && IS_AUDIO_STREAM_TYPE(pmt->streams[ii].stream_type))
        audio_pid = pid;

      if (video_pid && audio_pid)
        break;
    }
    free_pmt(&pmt);

    // Did we find what we want? If not, go round again and look for the
    // next PMT (subject to the number of records we're willing to search)
    if (video_pid && audio_pid)
      break;
  }

  if (!video_pid)
  {
    fprint_err("### No video streams specified in input file\n");
    free_TS_reader(&tsreader);
    return 1;
  }
  if (!audio_pid)
  {
    fprint_err("### No audio streams specified in input file\n");
    free_TS_reader(&tsreader);
    return 1;
  }

  if (!quiet)
  {
    fprint_msg("Extracting video PID %04x (%d)\n", video_pid, video_pid);
    fprint_msg("Extracting audio PID %04x (%d)\n", audio_pid, audio_pid);
  }

  err = extract_pids_packets(tsreader, output1, output2, video_pid, audio_pid, verbose, quiet);
  free_TS_reader(&tsreader);
  return err;
}

extern int demux(char *input_name,
                      char *output_name1,
                      char *output_name2)
{
  int input = open_binary_file(input_name, FALSE);
  if (input == -1)
  {
    fprint_err("### tsdemux: Unable to open input file %s\n", input_name);
    return 1;
  }

  FILE *output1 = fopen(output_name1, "wb");
  if (output1 == NULL)
  {
    (void) close_file(input);
    fprint_err("### tsdemux: Unable to open output file %s: %s\n", output_name1, strerror(errno));
    return 2;
  }

  FILE *output2 = fopen(output_name2, "wb");
  if (output2 == NULL)
  {
    (void) close_file(input);
    (void) fclose(output1);
    fprint_err("### tsdemux: Unable to open output file %s: %s\n", output_name2, strerror(errno));
    return 3;
  }

  int err = extract_av(input, output1, output2, FALSE, TRUE);
  if (err)
  {
    print_err("### tsdemux: Error extracting data\n");
    (void) close_file(input);
    (void) fclose(output1);
    (void) fclose(output2);
    return 4;
  }

  if (close_file(input)) {
    fprint_err("### tsdemux: Error closing input file %s\n", input_name);
    (void) fclose(output1);
    (void) fclose(output2);
    return 5;
  }
  if (fclose(output1))
  {
    fprint_err("### tsdemux: Error closing output file %s: %s\n", output_name1, strerror(errno));
    (void) fclose(output2);
    return 6;
  }
  if (fclose(output2))
  {
    fprint_err("### tsdemux: Error closing output file %s: %s\n", output_name2, strerror(errno));
    return 7;
  }
  return 0;
}
