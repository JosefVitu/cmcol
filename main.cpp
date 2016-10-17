/*
 * cmcol -- a music collection file creator
 * Copyright (C) 2011-2016  Josef Vitu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/xiphcomment.h>

#include <argp.h>
#include <ftw.h>

#include <cstdio>
#include <cstring>

#include <vector>
#include <string>
#include <algorithm>

#if defined(LIBXML_WRITER_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#define MY_ENCODING "UTF-8"

// ARGP START
const char* argp_program_version = "cmcol 1.0";
const char* argp_program_bug_address = "josef@vitu.org";
static char doc[] = "cmcol -- a music collection file creator";
static char args_doc[] = "INPUT_DIR";

static struct argp_option options[] = {
  {"output", 'o', "FILE", OPTION_ARG_OPTIONAL,
    "Output to FILE instead of standard output", 0},
  {0, 0, 0, 0, 0, 0}
};

struct arguments {
  char* args[1];
  char* output_file;
};

arguments args;
// ARGP END

// XML START
xmlTextWriterPtr writer = NULL;
// XML END

std::vector<std::string> files;

// Parse a single option
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
  arguments* arguments = static_cast<struct arguments*>(state->input);

  switch (key) {
    case 'o':
      arguments->output_file = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)
        argp_usage(state);
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1)
        argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc,
                           0, 0, 0};

void writeElement(const char* name, const TagLib::String& value) {
  if (!value.isEmpty()) {
    xmlTextWriterWriteElement(writer, BAD_CAST name,
        BAD_CAST value.toCString(true));
  }
}

void try_parse(const std::string& name) {
  TagLib::FileRef f(name.c_str(), false);
  if (!f.isNull()) {
    const std::string prefix(args.args[0]);
    std::string short_name(name);
    short_name.erase(0, prefix.size());

    xmlTextWriterStartElement(writer, BAD_CAST "file");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
        BAD_CAST short_name.c_str());
    xmlTextWriterEndAttribute(writer);

    const TagLib::Tag* tag = f.tag();
    if (!tag->isEmpty()) {
      writeElement("title", tag->title());
      writeElement("artist", tag->artist());
      writeElement("album", tag->album());
      char year[5];
      snprintf(year, sizeof(year), "%d", tag->year());
      if (strcmp(year, "0")) { // Actually empty
          writeElement("year", year);
      }
      char track[5];
      snprintf(track, sizeof(track), "%d", tag->track());
      if (strcmp(track, "0")) { // Actually empty
          writeElement("track", track);
      }
      writeElement("genre", tag->genre());
    }

    xmlTextWriterEndElement(writer);
  }
}

int list_dir(const char* name, const struct stat* /*status*/, int type, struct FTW* /*ftwbuf*/) {
  switch (type) {
    case FTW_F:
        // A file
        files.push_back(name);
        break;
    default:
      break;
  }

  return 0;
}

int parse_and_write(const char* input_dir, const char* output_file) {
  xmlDocPtr document;

  writer = xmlNewTextWriterDoc(&document, 0);  // No compression
  if (writer == NULL) {
    fprintf(stderr, "Error creating the XML writer.\n");
    xmlFreeDoc(document);
    return 1;
  }

  // Start the document
  xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);

  // Root element
  xmlTextWriterStartElement(writer, BAD_CAST "collection");

  xmlTextWriterWriteAttribute(writer, BAD_CAST "root",
      BAD_CAST input_dir);
  xmlTextWriterEndAttribute(writer);

  // Recursively process the input directory
  nftw(input_dir, list_dir, 10, 0);

  // Sort the list of files
  std::sort(files.begin(), files.end());

  // Parse each file
  std::for_each(files.begin(), files.end(), try_parse);

  // Close the document
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  FILE* output = NULL;

  if (strcmp(output_file, "-") == 0) {
    output = stdout;
  } else {
    output = fopen(output_file, "w");
  }

  if (output != NULL) {
    xmlDocFormatDump(output, document, 1);
    fclose(output);
  }

  xmlFreeDoc(document);
  return 0;
}

int main(int argc, char** argv) {
  int ret;
  args.output_file = const_cast<char*>("-");  // Deafult value

  argp_parse(&argp, argc, argv, 0, 0, &args);

  LIBXML_TEST_VERSION

  ret = parse_and_write(args.args[0], args.output_file);

  xmlCleanupParser();
  return ret;
}
#else
int main(void) {
  fprintf(stderr, "XML writer or output support not compiled in.\n");
  return 1;
}
#endif
