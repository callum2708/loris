#pragma once

#include <stdint.h>
#include "FileSystemNode.h"

struct dirent // One of these is returned by the readdir call, according to POSIX.
{
  char name[128]; // Filename.
  uint32_t ino;     // Inode number. Required by POSIX.
};

extern fs_node *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// not file nodes.
uint32_t read_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node *node, uint8_t read, uint8_t write);
void close_fs(fs_node *node);
dirent *readdir_fs(fs_node *node, uint32_t index);
fs_node *finddir_fs(fs_node *node, char *name);