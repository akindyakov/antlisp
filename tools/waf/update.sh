#!/usr/bin/env bash

set -x -e

WORK_DIR="./build-waf"
ROOT_DIR="./../"

go_to_work_dir() {
    while [[ ! -f LICENSE ]]; do
        cd ../
    done
    cd "${WORK_DIR}"
}

install() {
    local version="waf-2.0.8"
    local dir_name="${version}"
    local archive_filename="${dir_name}.tar.bz2"
    curl --output "${archive_filename}" "https://waf.io/${archive_filename}"
    tar -xjvf "${archive_filename}"
    cp "${dir_name}/waf" "${ROOT_DIR}"
}

go_to_work_dir
install
