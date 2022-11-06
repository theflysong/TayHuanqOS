/**
 * @file vfs.c
 * @author theflysong (song_of_the_fly@163.com)
 * @brief VFS
 * @version alpha-1.0.0
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022 TayhuangOS Development Team
 * SPDX-License-Identifier: LGPL-2.1-only
 * 
 */

#include <fs/vfs.h>
#include <fs/fs/fat32.h>

#include <heap.h>
#include <logging.h>

#include <string.h>

/**
 * @brief 获得文件系统魔数
 * 
 * @param fs 文件系统
 * @return 文件系统魔数 
 */
INLINE dword get_fs_magic(fs_t *fs) {
    return *(dword *)fs;
}

/**
 * @brief 获得文件系统函数表
 * 
 * @param fs 文件系统
 * @return 文件系统函数表
 */
INLINE fs_func_table *fs_funcs(fs_t *fs) {
    return *(fs_func_table **)(fs);
}

/**
 * @brief 打开文件系统结果
 * 
 */
typedef struct {
    fs_t *fs;
    const char *type;
} fs_result;

/**
 * @brief 打开文件系统
 * 
 * @param disk 磁盘
 * @param pbr 分区引导记录
 * @return 文件系统结果
 */
PRIVATE fs_result __open_fs(disk_t *disk, void *pbr) {
    // FAT32
    if (is_fat32_fs(pbr)) {
        return (fs_result) {
            .fs = open_fat32_fs(disk, pbr), 
            .type = "FAT32"
        };
    }
    // Unknown
    return (fs_result) {
        .fs = NULL,
        .type = "Unknown"
    };
}

PUBLIC vfs_t *open_fs(disk_t *disk) {
    char _pbr[512];
    void *pbr = _pbr;
    // 获取PBR
    read_sector(disk, 0, 1, pbr);
    vfs_t *vfs = malloc(sizeof(vfs));
    //打开FS
    fs_result result = __open_fs(disk, pbr);
    vfs->fs = result.fs;
    vfs->fs_type = result.type; 
    return vfs;
}

PUBLIC void close_fs(vfs_t *fs) {
    if (fs->fs != NULL) {
        fs_funcs(fs->fs)->close_fs(fs->fs);
    }
    free(fs);
}

PUBLIC const char *get_fs_type(vfs_t *fs) {
    return fs->fs_type;
}

/**
 * @brief fnode -> vfnode
 * 
 * @param fs 文件系统
 * @param file 文件节点
 * @param size 文件大小
 * @param name 文件名
 * @return vfnode
 */
INLINE vfnode *packvf(vfs_t *fs, fnode *file, size_t size, wchar *name) {
    vfnode *vf = malloc(sizeof(vfnode));
    vf->file = file;
    vf->size = size;
    vf->name = name;
    vf->fs = fs;
    return vf;
}

/**
 * @brief dnode -> vdnode
 * 
 * @param fs 文件系统
 * @param dir 目录节点
 * @param is_last 是否为最后一个目录
 * @param is_leaf 是否为叶子节点(文件)
 * @param is_root 是否为根目录
 * @param name 目录名
 * @param size 目录文件大小
 * @param file 目录文件节点
 * @return vdnode
 */
INLINE vdnode *packvd(vfs_t *fs, dnode *dir, bool is_last, bool is_leaf, bool is_root,
                     wchar *name, size_t size, fnode *file) {
    vdnode *vd = malloc(sizeof(vdnode));
    vd->dir = dir;
    vd->next = vd->parent = vd->child = NULL;
    vd->file = packvf(fs, file, size, name);
    vd->is_last = is_last;
    vd->is_leaf = is_leaf;
    vd->is_root = is_root;
    vd->fs = fs;
    return vd;
}

PUBLIC vdnode *get_root_dir(vfs_t *fs) {
    if (fs->root_dir == NULL) {
        dnode_result root = fs_funcs(fs->fs)->get_root(fs->fs);
        fs->root_dir = packvd(fs, root.dir, root.is_last, root.is_leaf, root.is_root,
                         root.name, root.filesz, root.file);
    }
    return fs->root_dir;
}

PUBLIC vfnode *get_dir_file(vdnode *dir) {
    if (dir->file == NULL) {
        LERROR ("GRUB2 Loader VFS", "NULL file of directory");
    }
    return dir->file;
}

PUBLIC vdnode *get_next_dnode(vdnode *dir) {
    if (dir->is_last) {
        return NULL;
    }
    if (dir->next == NULL) {
        dnode_result result = fs_funcs(dir->fs->fs)->get_next_dnode(dir->fs->fs, dir->dir);
        dir->next = packvd(dir->fs, result.dir, result.is_last, result.is_leaf, result.is_root,
            result.name, result.filesz, result.file);
    }
    return dir->next;
}

PUBLIC vdnode *get_child_dnode(vdnode *dir) {
    if (dir->is_leaf) {
        return NULL;
    }
    if (dir->child == NULL) {
        dnode_result result = fs_funcs(dir->fs->fs)->get_child_dnode(dir->fs->fs, dir->dir);
        dir->next = packvd(dir->fs, result.dir, result.is_last, result.is_leaf, result.is_root,
            result.name, result.filesz, result.file);
    }
    return dir->child;
}

PUBLIC void get_file_data(vfnode *file, void *buffer) {
    fs_funcs(file->fs->fs)->get_file_data(file->fs->fs, file->file, buffer);
}