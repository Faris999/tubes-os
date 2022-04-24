#include "header/testing.h"
#include "header/utils.h"
#include "header/terminal.h"
#include "header/filesystem.h"

void test() {
    test_mkdir();
}

bool assert(bool condition, char *message) {
    if (condition) {
        printString("[PASS] ");
        printString(message);
        printString("\r\n");
        return true;
    } else {
        printString("[FAIL] ");
        printString(message);
        printString("\r\n");
        return false;
    }
}

void resetFilesystems() {
    byte emptySector[512];

    clear(emptySector, 512);

    writeSector(emptySector, FS_MAP_SECTOR_NUMBER);
    writeSector(emptySector, FS_NODE_SECTOR_NUMBER);
    writeSector(emptySector, FS_NODE_SECTOR_NUMBER + 1);
    writeSector(emptySector, FS_SECTOR_SECTOR_NUMBER);

    fillMap();    
}

void test_mkdir() {
    struct node_filesystem node_fs_buffer;

    resetFilesystems();

    mkdir("folder1", FS_NODE_P_IDX_ROOT);
    mkdir("folder2", FS_NODE_P_IDX_ROOT);

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    assert(node_fs_buffer.nodes[0].parent_node_index == FS_NODE_P_IDX_ROOT,
        "parent_node_index of folder1 is not FS_NODE_P_IDX_ROOT");
    assert(strcmp(node_fs_buffer.nodes[0].name, "folder1"),
        "name of folder1 is not folder1");
    assert(node_fs_buffer.nodes[0].sector_entry_index == FS_NODE_S_IDX_FOLDER,
        "sector_entry_index of folder1 is not FS_NODE_S_IDX_FOLDER");

    assert(node_fs_buffer.nodes[1].parent_node_index == FS_NODE_P_IDX_ROOT,
        "parent_node_index of folder2 is not FS_NODE_P_IDX_ROOT");
    assert(strcmp(node_fs_buffer.nodes[1].name, "folder2"),
        "name of folder2 is not folder2");
    assert(node_fs_buffer.nodes[1].sector_entry_index == FS_NODE_S_IDX_FOLDER,
        "sector_entry_index of folder2 is not FS_NODE_S_IDX_FOLDER");
}