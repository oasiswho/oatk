/*********************************************************************************
 * MIT License                                                                   *
 *                                                                               *
 * Copyright (c) 2023 Chenxi Zhou <chnx.zhou@gmail.com>                          *
 *                                                                               *
 * Permission is hereby granted, free of charge, to any person obtaining a copy  *
 * of this software and associated documentation files (the "Software"), to deal *
 * in the Software without restriction, including without limitation the rights  *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     *
 * copies of the Software, and to permit persons to whom the Software is         *
 * furnished to do so, subject to the following conditions:                      *
 *                                                                               *
 * The above copyright notice and this permission notice shall be included in    *
 * all copies or substantial portions of the Software.                           *
 *                                                                               *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE *
 * SOFTWARE.                                                                     *
 *********************************************************************************/

/********************************** Revision History *****************************
 *                                                                               *
 * 03/02/23 - Chenxi Zhou: Created                                               *
 *                                                                               *
 *********************************************************************************/

#ifndef __PATH_H__
#define __PATH_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "khashl.h"

#include "graph.h"
#include "hmmannot.h"

KHASHL_MAP_INIT(KH_LOCAL, kh_u32_t, kh_u32, khint32_t, uint32_t, kh_hash_uint32, kh_eq_generic)

// GFA tag
extern char TAG_ARC_COV[4]; // arc coverage
extern char TAG_SEQ_COV[4]; // seq coverage
extern char TAG_SBP_COV[4]; // seq total base coverage

typedef struct {
    char *name; // seq id
    char *seq; // sequence
    uint32_t len; // seq length
    uint32_t cov; // fragment coverage
} asg_seg_t;

typedef struct {
    // seg and asmg vtx indices are always interchangeable
    // as we never do graph clean
    uint64_t m_seg, n_seg;
    asg_seg_t *seg; // sequence dictionary
    void *h_seg; // sequence hash map: name -> index
    asmg_t *asmg;
} asg_t;

typedef struct {
    // two preference criteria: length and circularisation
    // a path is 'best' if it is a Pareto optimium
    char *sid;
    uint32_t nv:30, circ:1, best:1;
    uint32_t *v;
    uint32_t len;
    double wlen;
    double srcc; // Spearman's rank correlation coefficient for gene order
} path_t;

typedef struct {size_t n, m; path_t *a;} path_v;

#ifdef __cplusplus
extern "C" {
#endif

asg_t *asg_init();
void asg_destroy(asg_t *g);
uint32_t asg_name2id(asg_t *g, char *name);
asg_t *asg_read(const char *fn);
asg_t *asg_make_copy(asg_t *g);
uint32_t asg_add_seg(asg_t *g, char *name, int allow_dups);
uint32_t asg_add_seg1(asg_t *g, char *name, char *seq, uint32_t len, uint64_t cov, int allow_dups);
void asg_subgraph(asg_t *asg, uint32_t *seeds, uint32_t n, uint32_t step);
void asg_stat(asg_t *asg, FILE *fo);
void asg_print(asg_t *g, FILE *fo, int no_seq);
void asg_print_fa(asg_t *g, FILE *fo, int line_wd);

void path_destroy(path_t *path);
double estimate_sequence_copy_number_from_coverage(asg_t *asg, int *copy_number, int max_copy);
int adjust_sequence_copy_number_by_graph_layout(asg_t *asg, int *copy_number, int max_copy);
kh_u32_t *sequence_duplication_by_copy_number(asg_t *asg, int *copy_number);
void graph_path_finder(asg_t *asg, kh_u32_t *seg_dups, path_v *paths);
path_t make_path_from_str(asg_t *asg, char *path_str, char *sid);
void path_sort(path_v *paths);
void path_rotate(asg_t *g, path_t *path, hmm_annot_v *annots, uint8_t og_type);
void path_stats(asg_t *asg, path_v *paths, FILE *fo);
uint32_t select_best_seq(asg_t *g, path_v *paths, FILE *fo, int type, double seq_cf, int seq_id, int cmp_coeff);
void print_seq(asg_t *asg, path_t *path, FILE *fo, int id, int force_linear, int line_wd);
void print_all_best_seqs(asg_t *g, path_v *paths, FILE *fo);
int is_valid_gfa_tag(const char *tag);

#ifdef __cplusplus
}
#endif

#endif

