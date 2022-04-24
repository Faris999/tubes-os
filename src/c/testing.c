#include "header/testing.h"
#include "header/utils.h"
#include "header/terminal.h"
#include "header/filesystem.h"

int tests_run = 0;
int passed_tests = 0;

void test() {
    test_mkdir();
    // test_rename();

    printString("\r\n");
    printHex(passed_tests);
    printString("/");
    printHex(tests_run);
    printString(" tests passed\r\n");
}

void testcaseA() {
    byte current_dir = 0xFF;
    printString("test case A\r\n");
    cd("folder1", &current_dir);
    printHex(current_dir);
    cd("folder4", &current_dir);
    printHex(current_dir);
    cd("folder5", &current_dir);
    printHex(current_dir);
    cd("..", &current_dir);
    printHex(current_dir);
    ls("", current_dir);
}

void testcaseB() {
    printString("test case B\r\n");
    ls("", 0xff);
}

void testcaseC() {
    printString("test case C\r\n");
    ls("", 0xff);
}

void testcaseD() {
    printString("test case D\r\n");
    ls("", 0xff);
}

bool assert(bool condition, char *message) {
    tests_run++;
    if (condition) {
        printString("[PASS] ");
        printString(message);
        printString("\r\n");
        passed_tests++;
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

    printString("Testing mkdir\r\n");

    resetFilesystems();

    mkdir("folder1", FS_NODE_P_IDX_ROOT);
    mkdir("folder2", FS_NODE_P_IDX_ROOT);

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    assert(node_fs_buffer.nodes[0].parent_node_index == FS_NODE_P_IDX_ROOT,
        "parent_node_index of folder1 is FS_NODE_P_IDX_ROOT");
    assert(strcmp(node_fs_buffer.nodes[0].name, "folder1"),
        "name of folder1 is folder1");
    assert(node_fs_buffer.nodes[0].sector_entry_index == FS_NODE_S_IDX_FOLDER,
        "sector_entry_index of folder1 is FS_NODE_S_IDX_FOLDER");

    assert(node_fs_buffer.nodes[1].parent_node_index == FS_NODE_P_IDX_ROOT,
        "parent_node_index of folder2 is FS_NODE_P_IDX_ROOT");
    assert(strcmp(node_fs_buffer.nodes[1].name, "folder2"),
        "name of folder2 is folder2");
    assert(node_fs_buffer.nodes[1].sector_entry_index == FS_NODE_S_IDX_FOLDER,
        "sector_entry_index of folder2 is FS_NODE_S_IDX_FOLDER");
}