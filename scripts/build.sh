#!/usr/bin/env bash

# path
src_dir="src"
bin_dir="bin"

c_sources=$(find "${src_dir}" -type f -name "*.c")

# project
project_name="gridline"
project="${bin_dir}/${project_name}"

c_compiler="clang"
c_opt=("-O0" "-g")
c_wrn=("-Wall" "-Wextra" "-pedantic" "-Wvla" "-Wc++-compat")
c_inc=("-I" "${src_dir}")

if [[ ! -e "${bin_dir}" ]]; then
  mkdir "${bin_dir}"
fi

for file in ${c_sources[@]}; do
  output_file=$(
    printf "${file}\n" |
    sed "s/^${src_dir}/${bin_dir}/" |
    sed "s/\.c\$/.o/")
  output_path=$(dirname "${output_file}")
  if [[ ! -d "${output_path}" ]]; then
    mkdir -p "${output_path}"
  fi
  if ! ${c_compiler} ${c_opt[@]} ${c_wrn[@]} ${c_inc[@]} -MJ "${output_file}.json" -c "${file}" -o "${output_file}"; then
    exit 0
  fi
done


c_objects=$(find "${bin_dir}" -type f -name "*.o" ! -path "*/${tst_dir}/*")
if ! ${c_compiler} ${c_opt[@]} ${c_wrn[@]} ${c_inc[@]} ${c_objects[@]} -o "${project}"; then
  exit 0
fi

databases=$(find "${bin_dir}" -type f -name "*.o.json")
sed -e '1s/^/[\'$'\n''/' -e '$s/,$/\'$'\n'']/' ${databases[@]} > compile_commands.json

