import argparse

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def import_data(path):
    return pd.read_csv(path, sep='\t', header=None)

def plot_gflops(ax, N, X, ERR, cores, lib, places_policy):
    count = 0
    for n, t, err in zip(N, X, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', linewidth=0.5, elinewidth=2, capsize=0,
                    label='{} ({})'.format(lib, places_policy))
        count += 1
    xlabel = "size"
    ax.set_xlabel(xlabel)
    ax.set_ylabel('gflops')
    ax.legend()

def get_gflops_stats(size, data):
    gflops = data.iloc[:, -1].values

    g_mean = np.empty(len(size))
    g_std = np.empty(len(size))
    for n in range(len(size)):
        g_n = gflops[np.where(data.iloc[:, 1].values == size[n])[0]]
        g_mean_n = np.mean(g_n)
        g_std_n = np.std(g_n)

        g_mean[n] = g_mean_n
        g_std[n] = g_std_n

    return g_mean, g_std

def main():
    parser = argparse.ArgumentParser(description='Parse args for plotting time data')
    parser.add_argument('--mkl_path', type=str, help='path to txt file')
    parser.add_argument('--oblas_path', type=str, help='path to txt file')
    args = parser.parse_args()

    places_policy = 'sockets\spread'

    mkl_all_data = import_data(args.mkl_path)
    mkl_groups = mkl_all_data.groupby([mkl_all_data.columns[0]])
    oblas_all_data = import_data(args.oblas_path)
    oblas_groups = oblas_all_data.groupby([oblas_all_data.columns[0]])
    N, G, ERR_G, cores = {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}
    for oblas_key, mkl_key in zip(list(oblas_groups.groups.keys()), list(mkl_groups.groups.keys())):
        mkl_data = mkl_groups.get_group(mkl_key)
        print('mkl grp\n', mkl_data)
        oblas_data = oblas_groups.get_group(oblas_key)
        print('oblas grp\n', oblas_data)

        assert all(np.unique(mkl_data.iloc[:, 1].values) == np.unique(oblas_data.iloc[:, 1].values))
        assert all(np.unique(mkl_data.iloc[:, 2].values) == np.unique(oblas_data.iloc[:, 2].values))
        assert all(np.unique(mkl_data.iloc[:, 3].values) == np.unique(oblas_data.iloc[:, 3].values))
        mnk = np.unique(mkl_data.iloc[:, 1].values)

        s_mean_mkl, s_std_mkl = get_gflops_stats(mnk, mkl_data)
        N['mkl'] += [mnk]
        G['mkl'] += [s_mean_mkl]
        ERR_G['mkl'] += [s_std_mkl]
        cores['mkl'] += [mkl_key]

        s_mean_oblas, s_std_oblas = get_gflops_stats(mnk, oblas_data)
        N['oblas'] += [mnk]
        G['oblas'] += [s_mean_oblas]
        ERR_G['oblas'] += [s_std_oblas]
        cores['oblas'] += [oblas_key]

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    plot_gflops(ax, N['mkl'], G['mkl'], ERR_G['mkl'], cores['mkl'], 'mkl', places_policy)
    plot_gflops(ax, N['oblas'], G['oblas'], ERR_G['oblas'], cores['oblas'], 'oblas', places_policy)
    plt.show()



if __name__ == '__main__':
    main()