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

#include <boost/program_options.hpp>

#include <ftw.h>

#include <cstdio>
#include <cstring>

#include <vector>
#include <string>
#include <algorithm>

#if defined(LIBXML_WRITER_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#define MY_ENCODING "UTF-8"

// XML START
xmlTextWriterPtr writer = NULL;
// XML END

std::string input_dir(".");
std::vector<std::string> files;
bool verbose = false;

void writeElement(const char* name, const TagLib::String& value) {
  if (!value.isEmpty()) {
    xmlTextWriterWriteElement(writer, BAD_CAST name,
        BAD_CAST value.toCString(true));
  }
}

void try_parse(const std::string& name) {
  TagLib::FileRef f(name.c_str(), false);
  if (!f.isNull()) {
    std::string short_name(name);
    short_name.erase(0, input_dir.size());

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
  // Program options
  namespace po = boost::program_options;
  std::string output_file("-"); // Defaults to standard output
  po::options_description desc("Options");
  desc.add_options()
    ("input,i", po::value<std::string>(),
     "Input directory (current directory if not specified)")
    ("output,o", po::value<std::string>(),
     "Output file (standard output if not specified)")
    ("verbose,v", "Increase verbosity (prints to standard output)")
    ("help,?", "Print this help message")
    ("version,V", "Print program version");

  po::positional_options_description pd;
  pd.add("input", -1);

  try {
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.empty() || vm.count("help")) {
      std::cout << "cmcol -- a music collection file creator\n\n"
        << desc
        << "\nReport bugs here: https://github.com/JosefVitu/cmcol/issues"
        << std::endl;
      return 0;
    }

    if (vm.count("version")) {
      std::cout << "cmcol 1.2.0" << std::endl;
      return 0;
    }

    if (vm.count("verbose")) {
      verbose = true;
    }

    if (vm.count("input")) {
      input_dir = vm["input"].as<std::string>();
    }

    char* actual_path = realpath(input_dir.c_str(), NULL);

    if (actual_path) {
      if (verbose) {
        std::cout << "Input: " << actual_path << std::endl;
      }

      free(actual_path);
    } else {
      throw std::invalid_argument("Input directory not found.");
    }

    if (vm.count("output")) {
      output_file = vm["output"].as<std::string>();
    }
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
  }

  LIBXML_TEST_VERSION

  const int ret = parse_and_write(input_dir.c_str(), output_file.c_str());

  xmlCleanupParser();
  return ret;
}
#else
int main(void) {
  fprintf(stderr, "XML writer or output support not compiled in.\n");
  return 1;
}
#endif
