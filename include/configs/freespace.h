#ifndef CONFIG_FREE_SPACE_H
#define CONFIG_FREE_SPACE_H

#define FreeSpaceBLRange 0xF635C
#define FreeSpace 0x7FF0A4

/**
 * ----------------------------------------------------------
 * USAGE       | BASE      | SIZE      | Comment
 * ----------------------------------------------------------
 * Magic       | 0x07FF0A4 | 0x10      | "MOKHA FE6 CBUILD"
 * Reloc       | 0x07FF0B4 | -------   |
 * Main        | -------   | -------   |
 * Fonts       | *WIP*     | *WIP*     |
 * ----------------------------------------------------------
 */

#define FreeSpaceRelocSize 0x200


#endif /* CONFIG_FREE_SPACE_H */
