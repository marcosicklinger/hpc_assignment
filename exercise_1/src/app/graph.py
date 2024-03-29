import argparse
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

NP = 2
NT = 3


def import_time_data(path):
    return pd.read_csv(path, sep='\t')


def plot_speedup(N, T, ERR, size, mod):
    fig, ax = plt.subplots(1, 1, figsize=(5, 4.5))
    count = 0
    for n, t, err in zip(N, T, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', markersize=7.5, linewidth=1.5, elinewidth=2, capsize=0,
                    label='{}x{}'.format(size[count][0], size[count][1]))
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(0)
    ax.plot([i for i in range(1, np.max(N[0]) + 1)], [i for i in range(1, np.max(N[0]) + 1)],
            color='red', label='ideal', linestyle='dashed')
    xlabel = "mpi tasks" if mod == 2 else "omp threads"
    ax.set_xlabel(xlabel, fontsize=14)
    ax.set_ylabel('speedup', fontsize=14)
    ax.legend()
    plt.show()


def plot_time(N, T, ERR, size, mod):
    fig, ax = plt.subplots(1, 1, figsize=(5, 4.5))
    count = 0
    for n, t, err in zip(N, T, ERR):
        # ax.plot(N, t)
        line, _, _ = ax.errorbar(n, t, err, marker='s', markersize=7.5, linewidth=1.5, elinewidth=2, capsize=0,
                                 label='{}x{}'.format(size[count][0], size[count][1]))
        # ax.plot([i for i in range(1, np.max(N[0]) + 1)], [T[count][0] / i for i in range(1, np.max(N[0]) + 1)],
        #         color=line.get_color(), label='{}x{} ideal'.format(size[count][0], size[count][1]), linestyle='dashed')
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(0)
    xlabel = "mpi tasks" if mod == 2 else "omp threads"
    ax.set_xlabel(xlabel, fontsize=14)
    ax.set_ylabel('time (s)', fontsize=14)
    ax.legend()
    plt.show()


def plot_weak(N, T, ERR, size, mod):
    fig, ax = plt.subplots(1, 1, figsize=(5, 4.5))
    count = 0
    for n, t, err in zip(N, T, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', markersize=7.5, linewidth=1.5, elinewidth=2, capsize=0,
                    label='{}x{} threads'.format(size[count][0], size[count][1]))
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(0)
    ax.set_ylim([0, 100])
    xlabel = "mpi tasks"
    ax.set_xlabel(xlabel, fontsize=14)
    ax.set_ylabel('time (s)', fontsize=14)
    # ax.legend()
    plt.show()


def get_time_stats(punits, mod, data):
    t = data.iloc[:, -1].values

    t_mean = np.empty(len(punits))
    std = np.empty(len(punits))
    for n in range(len(punits)):
        t_n = t[np.where(data.iloc[:, mod].values == punits[n])[0]]
        t_mean[n] = np.mean(t_n)
        std[n] = np.std(t_n)

    return t_mean, std


def get_speedup_stats(punits, mod, data):
    t = data.iloc[:, -1].values
    t1 = t[np.where(data.iloc[:, mod].values == 1)[0]]
    t1_mean = np.mean(t1)
    t1_std = np.std(t1)

    s_mean = np.empty(len(punits))
    s_std = np.empty(len(punits))
    for n in range(len(punits)):
        t_n = t[np.where(data.iloc[:, mod].values == punits[n])[0]]
        t_mean = np.mean(t_n)
        t_std = np.std(t_n)

        s_mean[n] = t1_mean / t_mean
        s_std[n] = s_mean[n] * (t1_std / t1_mean + t_std / t_mean)

    return s_mean, s_std


def main():
    parser = argparse.ArgumentParser(description='Parse args for plotting time data')
    parser.add_argument('--path', type=str, help='path to txt file')
    args = parser.parse_args()

    all_data = import_time_data(args.path)
    size_groups = all_data.groupby([all_data.columns[i] for i in range(2)])
    N, T, S, ERR_T, ERR_S, size = [], [], [], [], [], []
    mod = None
    other_grp = False
    for group_key in list(size_groups.groups.keys()):
        data = size_groups.get_group(group_key)
        punits, mod = None, None
        if len(np.unique(data.iloc[:, NP].values)) == 1 and len(np.unique(data.iloc[:, NT].values)) > 1:
            punits = np.unique(data.iloc[:, NT].values)
            mod = NT
        elif len(np.unique(data.iloc[:, NT].values)) == 1 and len(np.unique(data.iloc[:, NP].values)) > 1:
            punits = np.unique(data.iloc[:, NP].values)
            mod = NP
        else:
            other_grp = True
            break

        t_mean, t_std = get_time_stats(punits, mod, data)
        s_mean, s_std = get_speedup_stats(punits, mod, data)
        N += [punits]
        T += [t_mean]
        S += [s_mean]
        ERR_T += [t_std]
        ERR_S += [s_std]
        size += [group_key]
    if not other_grp:
        plot_speedup(N, S, ERR_S, size, mod)
        plot_time(N, T, ERR_T, size, mod)

    th_groups = all_data.groupby([all_data.columns[3]])
    for group_key in list(th_groups.groups.keys()):
        data = th_groups.get_group(group_key)
        punits, mod = None, None
        if len(np.unique(data.iloc[:, NT].values)) == 1 and len(np.unique(data.iloc[:, NP].values)) > 1:
            punits = np.unique(data.iloc[:, NP].values)
            mod = NP
        else:
            break

        s_mean, s_std = get_speedup_stats(punits, mod, data)
        N += [punits]
        S += [s_mean]
        ERR_S += [s_std]

        t_mean, t_std = get_time_stats(punits, mod, data)
        N += [punits]
        T += [t_mean]
        ERR_T += [t_std]

        size += [group_key]


    if other_grp:
        plot_weak(N, S, ERR_S, size, mod)
        plot_weak(N, T, ERR_T, size, mod)

    if not other_grp:
        pass
    else:
        pass


if __name__ == '__main__':
    main()
