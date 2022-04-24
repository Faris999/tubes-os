#include "header/testing.h"

#include "header/filesystem.h"
#include "header/terminal.h"
#include "header/utils.h"

int tests_run = 0;
int passed_tests = 0;

void print_passed_tests() {
  printString("\r\n");
  printString("Passed tests: ");
  printInt(passed_tests);
  printString("/");
  printInt(tests_run);
  printString("\r\n");
}

void test() {
  test_mkdir();
  // test_rename();

  print_passed_tests(); 
}

void testcaseA() {
  byte current_dir = 0xFF;
  printString("test case A\r\n");
  cd("folder1", &current_dir);
  assert(current_dir == 0x0, "cd folder1");
  cd("folder4", &current_dir);
  assert(current_dir == 0x6, "cd folder4");
  cd("folder5", &current_dir);
  assert(current_dir == 0x7, "cd folder5");
  cd("..", &current_dir);
  assert(current_dir == 0x6, "cd ..");
  ls("", current_dir);

  print_passed_tests();
}

void testcaseB() {
  byte current_dir = 0xFF;
  printString("test case B\r\n");
  cat("file_idx_0", current_dir);
  cat("file_idx_63", current_dir);
  mv("file_idx_0", "folder1", current_dir);
  mv("file_idx_63", "folder1", current_dir);
}

void testcaseC() {
  printString("test case C\r\n");
  mkdir("test", 0xFF);
  mkdir("A2", 0xFF);
  mkdir("C11", 0xFF);
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