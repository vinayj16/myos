#include "fs.h"
#include "mm/mm.h"
#include "string.h"
#include "stdio.h"

// The root of the filesystem
fs_node_t *fs_root = NULL;

// Temporary storage for our initial filesystem
#define MAX_NODES 64
static fs_node_t nodes[MAX_NODES];
static uint32_t num_nodes = 0;

// Initialize the filesystem
void fs_init(void) {
    // Clear the nodes array
    memset(nodes, 0, sizeof(nodes));
    num_nodes = 0;
    
    // Create the root directory
    fs_root = make_fs_node("", FS_DIRECTORY, 0);
    
    // Create some default directories
    fs_node_t *etc = make_fs_node("etc", FS_DIRECTORY, 0);
    fs_node_t *home = make_fs_node("home", FS_DIRECTORY, 0);
    fs_node_t *bin = make_fs_node("bin", FS_DIRECTORY, 0);
    
    // Add them to the root directory
    mount_fs("etc", etc);
    mount_fs("home", home);
    mount_fs("bin", bin);
    
    printf("Filesystem initialized\n");
}

// Create a new filesystem node
fs_node_t *make_fs_node(char *name, uint32_t flags, uint32_t mask) {
    if (num_nodes >= MAX_NODES) {
        printf("make_fs_node: Maximum number of nodes reached\n");
        return NULL;
    }
    
    fs_node_t *node = &nodes[num_nodes++];
    
    strncpy(node->name, name, 127);
    node->name[127] = '\0';
    
    node->mask = mask;
    node->uid = 0;  // root
    node->gid = 0;  // root
    node->flags = flags;
    node->inode = num_nodes;
    node->length = 0;
    node->impl = 0;
    
    // Set default handlers
    node->read = 0;
    node->write = 0;
    node->open = 0;
    node->close = 0;
    node->readdir = 0;
    node->finddir = 0;
    node->ptr = 0;
    
    return node;
}

// Mount a filesystem at the given mountpoint
void mount_fs(char *mountpoint, fs_node_t *node) {
    if (!fs_root || !node) return;
    
    // Find the mountpoint in the root directory
    fs_node_t *mount_dir = finddir_fs(fs_root, mountpoint);
    if (!mount_dir) {
        printf("mount_fs: Mount point %s not found\n", mountpoint);
        return;
    }
    
    // Set up the mount
    mount_dir->ptr = node;
    mount_dir->flags |= FS_MOUNTPOINT;
    
    printf("Mounted filesystem at /%s\n", mountpoint);
}

// Read from a file
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (!node) return 0;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        return read_fs(node->ptr, offset, size, buffer);
    }
    
    // Call the node's read function if it exists
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }
    
    return 0;
}

// Write to a file
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (!node) return 0;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        return write_fs(node->ptr, offset, size, buffer);
    }
    
    // Call the node's write function if it exists
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }
    
    return 0;
}

// Open a file
void open_fs(fs_node_t *node, uint8_t read, uint8_t write) {
    if (!node) return;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        open_fs(node->ptr, read, write);
        return;
    }
    
    // Call the node's open function if it exists
    if (node->open) {
        node->open(node);
    }
}

// Close a file
void close_fs(fs_node_t *node) {
    if (!node) return;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        close_fs(node->ptr);
        return;
    }
    
    // Call the node's close function if it exists
    if (node->close) {
        node->close(node);
    }
}

// Read a directory entry
struct dirent *readdir_fs(fs_node_t *node, uint32_t index) {
    if (!node) return NULL;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        return readdir_fs(node->ptr, index);
    }
    
    // Call the node's readdir function if it exists
    if (node->readdir) {
        return node->readdir(node, index);
    }
    
    return NULL;
}

// Find a file in a directory
fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if (!node || !name) return NULL;
    
    // Handle mountpoints
    if ((node->flags & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
        return finddir_fs(node->ptr, name);
    }
    
    // Call the node's finddir function if it exists
    if (node->finddir) {
        return node->finddir(node, name);
    }
    
    // Simple linear search for now
    for (uint32_t i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].name, name) == 0) {
            return &nodes[i];
        }
    }
    
    return NULL;
}
