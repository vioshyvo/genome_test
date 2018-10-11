import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import sys
import glob
from matplotlib.backends.backend_pdf import PdfPages

plt.rcParams['font.family'] = 'Arial'
plt.rcParams['xtick.labelsize'] = 18
plt.rcParams['ytick.labelsize'] = 18


def pareto_frontier(Xs, Ys, maxX=True, maxY=True):
    myList = sorted([[Xs[i], Ys[i]] for i in range(len(Xs))], reverse=maxX)
    p_front = [myList[0]]
    for pair in myList[1:]:
        if maxY:
            if pair[1] >= p_front[-1][1]:
                p_front.append(pair)
        else:
            if pair[1] <= p_front[-1][1]:
                p_front.append(pair)
    p_frontX, p_frontY = [], []
    edX = -1
    for pair in reversed(p_front):
        if pair[0] < 0.1 or pair[0] - edX <= 0.001:
            continue
        edX = pair[0]
        p_frontX.append(pair[0])
        p_frontY.append(pair[1])
    return p_frontX, p_frontY


def main(k, files):
    n_test = 50
    legend = True
    save = False
    log = True
    set_ylim = True
    legend_label = 'filename' # 'sparsity', 'depth' or 'filename'
    show_title = True

    # ylim = (0,100 / n_test)
    ylim = (0,.01) # mnist data
    file_name = 'images/depth.png'
    title = 'Ecol data set, n = 1500, d = 23223411'
    exact_time = 21 # 50 test set points x approximately 22 seconds

    fig = plt.figure()
    LSD = []
    q = False
    A = []
    ax = plt.gca()

    for resfile in files:
        with open(resfile) as f:
            _ = f.readline()
            lines = [[float(x) for x in s.strip().split()] for s in f]
        acc = [x[5] for x in lines if x[0] == k]
        tym = [x[7] for x in lines if x[0] == k]
        # A.append((resfile.split('.')[0], acc, tym))
        A.append((lines[0][3], acc, tym, lines[0][2]))
    colors = cm.rainbow(np.linspace(0, 1, len(A)))
    minY, maxY = float('inf'), -float('inf')
    for a, c, m in zip(A, colors, ['>', 'v', 'd', '^', 'o', 'p', 'h', '<']):
        par = pareto_frontier(a[1], a[2], True, False)
        query_times = par[1]
        times_per_point = [x / n_test for x in query_times] # divide by the number of query points
        l, = ax.plot(par[0], times_per_point, linestyle='solid', marker=m, label=a[0], c=c, markersize=7)
        if q: LSD.append(l)
        maxY = max(maxY, max(times_per_point))
        accuracy_time_list = zip(times_per_point, par[0])
        minY = min(minY, min(x for x, y in accuracy_time_list if y >= 0.5))
    ax.semilogy()
    ax.set_ylabel('time (s)', fontsize=20)
    ax.set_xlabel('recall', fontsize=20)
    ax.set_xlim((0, 1))

    for pair in accuracy_time_list:
        print pair

    if show_title:
        ax.set_title(title, fontsize=20, y=1.05)

    if set_ylim:
        ax.set_ylim(ylim)
    else:
        ax.set_ylim((minY / 1.25, max(maxY * 1.25, exact_time * 1.15)))

    ax.xaxis.labelpad = 15
    ax.yaxis.labelpad = 15

    if log:
        ax.set_yscale('log')
    else:
        ax.set_yscale('linear')

    # ax.set_yticks(np.linspace(0, 1, 100))

    legend_idx = 0 if legend_label == 'sparsity' else 3
    labels = files if legend_label == 'filename' else [a[legend_idx] for a in A] 
    if legend:
        ax.legend(LSD, labels=labels, loc="upper left", title = legend_label)

    plt.axhline(y = exact_time, xmin = 0, xmax = 1, hold = None, linestyle = '--', color = 'red')

    if save:
        plt.savefig(file_name, bbox_inches='tight')
    else:
        plt.show()

if __name__ == '__main__':
    main(int(sys.argv[1]), sys.argv[2:])
