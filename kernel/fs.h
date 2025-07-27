#ifndef _FS_H
#define _FS_H

#include <stdint.h>
#include <stdbool.h>

// File types
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

// File system node
struct fs_node;

typedef uint32_t (*read_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*, uint32_t);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*, char *name);

typedef struct fs_node {
    char name[128];               // Filename
    uint32_t mask;                // Permission mask
    uint32_t uid;                 // Owning user
    uint32_t gid;                 // Owning group
    uint32_t flags;               // File type
    uint32_t inode;               // Inode number
    uint32_t length;              // Size in bytes
    uint32_t impl;                // Implementation-defined number
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr;          // Used by mountpoints and symlinks
} fs_node_t;

// Directory entry
struct dirent {
    char name[128];               // Filename
    uint32_t ino;                 // Inode number
};

// Standard file operations
extern fs_node_t *fs_root;        // The root of the filesystem

// Read from a file
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

// Write to a file
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

// Open a file
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);

// Close a file
void close_fs(fs_node_t *node);

// Read a directory entry
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);

// Find a file in a directory
fs_node_t *finddir_fs(fs_node_t *node, char *name);

// Initialize the filesystem
void fs_init(void);

// Create a new filesystem node
fs_node_t *make_fs_node(char *name, uint32_t flags, uint32_t mask);

// Mount a filesystem
void mount_fs(char *mountpoint, fs_node_t *node);

#endif // _FS_H
