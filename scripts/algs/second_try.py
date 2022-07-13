from copy import deepcopy

n , k = 16, 12
fail_id = 0
bw_path = 'bandwidths.txt'

MIN_BW = 10
MAX_BW = 1000

def get_bandwidth(path):
    data = [line.strip() for line in open(path)]
    data = [[int(b) for b in line.split(' ') if b != ''] for line in data]
    return zip(data[3::3], data[4::3])

def get_capacity(upload, download, kk, rid):
    #fix the bandwidth
    for i in range(len(upload)):
        if min(upload[i], download[i]) <= MIN_BW:
            upload[i] = 0
            download[i] = 0
    upload[rid] = 0
    download[rid] = MAX_BW
    #get upload limit
    exceeded = 0
    while (sum(upload) + exceeded < max(upload) * (kk - exceeded)):
        for i in range(len(upload)):
            if (sum(upload) + exceeded < upload[i] * (kk - exceeded)):
                upload[i] = -1
                exceeded += 1
    upload_limit = min((sum(upload) + exceeded) // (kk - exceeded), MAX_BW)
    upload = [upload_limit if u == -1 else u for u in upload]
    #get download limit
    while True:
        for i, u in enumerate(upload):
            if i != rid and download[i] > upload[i] * (kk - 1):
                download[i] = upload[i] * (kk - 1)
        download_limit = sum(download) // kk
        if max(upload) <= download_limit:
            break
        upload = [min(download_limit, u) for u in upload]
    #result
    return upload, download, min(upload_limit, download_limit)

class Node:
    def __init__(self, i, u, d):
        self.nid = i
        self.upload = u
        self.download = d
        self.task = Task(-1, 0, 0, 0)
        self.uptasks = []
    def __repr__(self):
        return (f'{self.nid}: {self.upload}, {self.download}, {self.task}'
                f' == {self.uptasks}')
    def add_task(self, task, size):
        tt = deepcopy(task)
        for uptask in self.uptasks:
            if uptask.tid == task.tid:
                if uptask.start == task.start:
                    ss = min(size, task.end - uptask.end)
                    task.end -= ss
                    tt.start = task.end
                else:
                    ss = min(size, uptask.start - task.start)
                    task.start += ss
                    tt.end = task.start
                break
        else:
            ss = min(size, task.size())
            task.start += ss
            tt.end = task.start
        if ss > 0:
            self.uptasks.append(tt)
        return ss

class Task:
    def __init__(self, i, s, e, n):
        self.tid = i
        self.start = s
        self.end = e
        self.node = n
    def __repr__(self):
        return f'{self.tid}-[{self.start} - {self.end}]({self.node})'\
                if self.tid != -1 else 'None'
    def size(self):
        return self.end - self.start

def assign_downloads(nodes, capacity, kk, rid):
    tasks = []
    current = 0
    nodes.sort(key=lambda x: (x.upload - x.download//(kk-1)), reverse=True)
    for i, node in enumerate(nodes):
        use = min(node.download // (kk - 1), capacity - current)
        node.task = Task(i, current, current + use, node.nid)
        tasks.append(deepcopy(node.task))
        current += use
        if current == capacity:
            break
    if current < capacity:
        tasks.insert(0, Task(-1, current, capacity, rid))
        nodes.insert(0, Node(rid, 0, 0))
        nodes[0].task = Task(-1, current, capacity, rid)
        for task in tasks:
            task.tid += 1
        for node in nodes:
            node.task.tid += 1
    return tasks

def assign_uploads(nodes, tasks, kk):
    count = kk
    finished = []
    waited = []
    aduse = []
    for i, node in enumerate(nodes):
        upload = node.upload
        remain = node.task.size()
        while upload > 0:
            if len(finished) == 0 and len(waited) == 0:
                if count == 0:
                    return
                j = i if remain > 0 else i + 1
                finished = deepcopy(tasks[:j])
                while len(aduse) > 0:
                    tt = aduse.pop()
                    if tt.size() > 0:
                        finished[tt.tid] = tt
                    else:
                        finished.pop(tt.tid)
                waited = deepcopy(tasks[j:])
                count -= 1
            if remain > 0:
                if len(waited) == 0:
                    tt = deepcopy(node.task)
                    aduse.append(tt)
                else:
                    tt = waited.pop(0)
                    if tt.size() > remain:
                        finished.append(tt)
                ss = node.add_task(tt, remain)
                remain -= ss
                upload -= ss
            else:
                for fi, fin in enumerate(finished):
                    ss = node.add_task(fin, upload)
                    if ss != 0:
                        upload -= ss
                        if fin.size() == 0:
                            finished.pop(fi)
                        break
                else:
                    if count == 0 or len(waited) == 0:
                        break
                    for wi in range(len(waited)):
                        ss = node.add_task(waited[-1-wi], upload)
                        upload -= ss
                        if waited[-1-wi].size() == 0:
                            waited.pop(-1-wi)
                            break

if __name__ == '__main__':
    for upload, download in get_bandwidth(bw_path):
        u, d, c = get_capacity(upload[:n], download[:n], k, fail_id)
        nodes = [Node(i, ud[0], ud[1]) for i, ud in enumerate(zip(u, d))
                 if min(ud) > 0]
        tasks = assign_downloads(nodes, c, k, fail_id)
        assign_uploads(nodes, tasks, k)
        ts = []
        for task in tasks:
            ts.append(task.size() * k)
        for node in nodes:
            ut = node.uptasks
            ut.sort(key=lambda x: (x.tid, x.start))
            for i, uut in enumerate(ut):
                if i != 0 and uut.tid == ut[i-1].tid and uut.start < ut[i-1].end:
                    print('???')
                    print(ut)
                    exit(-1)
                ts[uut.tid] -= uut.size()
        for tts in ts:
            if tts != 0:
                print('kkk', tts)
                for nnn in nodes:
                    print(nnn)
                exit(-1)
