#!/bin/sh

watcom="/opt/watcom/"

if [ ! -d $watcom ]; then
  echo "OpenWatcom not found at $watcom" >&2
  exit 1
fi

export WATCOM=$watcom
export EDPATH="$watcom/eddat"
export WIPFC="$watcom/wipfc"

export INCLUDE="$watcom/h"
export PATH="$watcom/binl64:$watcom/binl:$PATH"
