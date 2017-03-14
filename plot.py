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
    fig = plt.figure()
    LSD = []
    q = False
    A = []
    ax = plt.gca()
    for resfile in files:
        with open(resfile) as f:
            _ = f.readline()
            lines = [[float(x) for x in s.strip().split()] for s in f]
        acc = [x[-3] for x in lines if x[0] == k]
        tym = [x[-1] for x in lines if x[0] == k]
        A.append((resfile.split('.')[0], acc, tym))
    colors = cm.rainbow(np.linspace(0, 1, len(A)))
    minY, maxY = float('inf'), -float('inf')
    for a, c, m in zip(A, colors, ['>', 'v', 'd', '^', 'o', 'p', 'h', '<']):
        par = pareto_frontier(a[1], a[2], True, False)
        l, = ax.plot(par[0], par[1], linestyle='solid', marker=m, label=a[0], c=c, markersize=7)
        if q: LSD.append(l)
        maxY = max(maxY, max(par[1]))
        minY = min(minY, min(x for x, y in zip(par[1], par[0]) if y >= 0.5))
    ax.semilogy()
    ax.set_ylabel('time (s)', fontsize=22)
    ax.set_xlabel('recall', fontsize=22)
    ax.set_xlim((0, 1))
    # ax.set_ylim((minY / 1.25, maxY * 1.25))
    ax.set_ylim((0,1))
    ax.xaxis.labelpad = 15
    ax.yaxis.labelpad = 15
    ax.set_yscale('linear')
    # ax.set_title('title', fontsize=28, y=1.05)
    # ax.set_yticks(np.linspace(0, 1, 100))

	
    	
    # ax.legend(LSD, labels=[a[0] for a in A], loc="upper left")
    plt.show()
    # plt.savefig('plot.png', bbox_inches='tight')


if __name__ == '__main__':
    main(int(sys.argv[1]), sys.argv[2:])
