#!/bin/bash
set -ue

if [[ ! $# -eq 1 ]]; then
  echo "usage: $0 <output_dir>" >&2
  exit 1
fi

export output_dir="$1"

if [[ ! -d "${output_dir}" ]]; then
  echo "error: directory does not exist: ${output_dir}" >&2
  exit 1
fi

cp extra/web/documentation.css "${output_dir}"
./extra/web/build_api_reference.py
./extra/web/build_examples.py
./extra/web/build_pages.py
find "${output_dir}" -name "_*" -delete

