#!/bin/bash
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso -d int -no-reboot -M smm=off