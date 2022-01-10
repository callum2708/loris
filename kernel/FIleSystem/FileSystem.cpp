#include "FileSystem.h"

fs_node *fs_root = 0; // The root of the filesystem.

uint32_t read_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->read)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

uint32_t write_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->write)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void close_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->close)
        node->close(node);
}

void open_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->open)
        node->open(node);
}

dirent *readdir_fs(fs_node *node, uint32_t index)
{
    if ((node->flags & 0x7) != FS_DIRECTORY || !node->readdir)
        return 0;
    if (node->readdir)
        return node->readdir(node, index);
    else
        return 0;
}

fs_node *finddir_fs(fs_node *node, char *name)
{
    if ((node->flags & 0x7) != FS_DIRECTORY || !node->readdir)
        return 0;
    if (node->finddir)
        return node->finddir(node, name);
    else
        return 0;
}