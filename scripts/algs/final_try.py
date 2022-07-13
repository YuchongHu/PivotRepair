from copy import deepcopy

n , k = 6, 4
fail_id = 0
bw_path = 'bandwidths.txt'

MIN_BW = 50000
MAX_BW = 1000000

def get_bandwidth(path):
    data = [line.strip() for line in open(path)]
    data = [[int(b)*1000 for b in line.split(' ') if b != ''] for line in data]
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
    def get_cross(self, task):
        result = []
        for uptask in self.uptasks:
            if uptask.tid != task.tid:
                continue
            if uptask.end <= task.start or uptask.start >= task.end:
                continue
            tt = deepcopy(task)
            tt.start = max(uptask.start, task.start)
            tt.end = min(uptask.end, task.end)
            result.append(tt)
        return result, sum([tt.size() for tt in result])
    def delete(self, task):
        need_delete = []
        need_add = []
        for i, uptask in enumerate(self.uptasks):
            if uptask.tid != task.tid:
                continue
            if uptask.end <= task.start or uptask.start >= task.end:
                continue
            need_delete.append(i)
            temp = min(task.start, uptask.end)
            if temp > uptask.start:
                tt = deepcopy(uptask)
                tt.end = temp
                need_add.append(tt)
            temp = max(task.end, uptask.start)
            if temp < uptask.end:
                tt = deepcopy(uptask)
                tt.start = temp
                need_add.append(tt)
        need_delete.reverse()
        for i in need_delete:
            self.uptasks.pop(i)
        for tt in need_add:
            self.uptasks.append(tt)

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
    nodes.sort(key=lambda x: (x.download, x.upload, x.nid), reverse=True)
    dsum = sum([x.download for x in nodes])
    for node in nodes:
        use = min(node.download * capacity // dsum, capacity - current)
        node.task = Task(0, 0, use, node.nid)
        current += use
        if current == capacity:
            break
    current = 0
    cur_id = 0
    nodes.sort(key=lambda x: (x.upload - x.task.size()), reverse=True)
    for node in nodes:
        ss = node.task.size()
        if node.task.size() > 0:
            node.task = Task(cur_id, current, current + ss, node.nid)
            tasks.append(deepcopy(node.task))
            current += ss
            cur_id += 1
    if current < capacity:
        tasks.insert(0, Task(-1, current, capacity, rid))
        nodes.insert(0, Node(rid, 0, 0))
        nodes[0].task = Task(-1, current, capacity, rid)
        for task in tasks:
            task.tid += 1
        for node in nodes:
            if node.task.size() > 0:
                node.task.tid += 1
    return tasks

def assign_uploads(nodes, tasks, kk):
    remains = [[deepcopy(task), kk] for task in tasks]
    cur_id = 0
    for ni, node in enumerate(nodes):
        assign_node(cur_id, node, remains, tasks, nodes, ni)
        if node.task.size() > 0:
            cur_id += 1

def assign_node(i, node, remains, tasks, nodes, ni):
    if node.task.size() > 0:
        remains.sort(key=lambda x: x[0].tid)
        remains[i][1] -= 1
        node.uptasks.append(node.task)
    upload = node.upload - node.task.size()
    #for nnn in nodes:
        #print(nnn)
    #print(remains)
    #print(i, node.nid, upload)
    #input('\n')
    while upload > 0:
        remains.sort(key=lambda x: (x[1], x[0].tid < i, x[0].tid), reverse=True)
        for remain in remains:
            if remain[1] == 0:
                continue
            elif (remain[0].tid == i and node.task.size() > 0) or \
                 (remain[1] == 1 and \
                  ((remain[0].tid > i and node.task.size() > 0) or \
                   (remain[0].tid >= i and node.task.size() == 0))):
                continue
            ss = node.add_task(remain[0], upload)
            if ss > 0:
                upload -= ss
                if remain[0].size() == 0:
                    remain[0] = deepcopy(tasks[remain[0].tid])
                    remain[1] -= 1
                #for nnn in nodes:
                    #print(nnn)
                #print(remains)
                #print(i, node.nid, upload)
                #input('\n')
                break
        else:
            break
    remains.sort(key=lambda x: x[0].tid)
    if upload > 0 and i+1 < len(remains) and node.task.size() > 0 and \
       remains[i][1] == 1 and remains[i+1][1] ==1:
        #for nnn in nodes:
            #print(nnn)
        #print(remains)
        #print(i, node.nid, upload)
        #input('\n')
        tt = deepcopy(remains[i][0])
        ss = min(tt.size(), upload)
        tt.end = tt.start + ss
        remains[i][0].start += ss
        kks = []
        for remain in remains[i+1:]:
            kk = deepcopy(remain[0])
            kss = min(kk.size(), ss)
            kk.end = kk.start + kss
            ss -= kk.size()
            kks.append(kk)
            if ss == 0:
                break
        while tt.size() > 0:
            for nnn in nodes[:ni]:
                tcross, tcsize = nnn.get_cross(tt)
                kcross, kcsize = nnn.get_cross(kks[0])
                tcross.sort(key=lambda x: x.start)
                kcross.sort(key=lambda x: x.start)
                if tcsize < tt.size() and kcsize > 0:
                    if len(tcross) > 0:
                        if tcross[0].start == tt.start or kcross[0].start != kks[0].start:
                            continue
                        tcsize = tt.end - tcross[0].start
                    #print(nnn.nid, nnn.uptasks)
                    #print(tt, tcross)
                    #print(kks, kcross)
                    #input('\n')
                    chsize = min(tt.size() - tcsize, kcross[0].size())
                    #add
                    tadd = deepcopy(tt)
                    tadd.end = tadd.start + chsize
                    tt.start = tadd.end
                    nnn.uptasks.append(tadd)
                    #exchange
                    tex = kcross[0]
                    tex.end = tex.start + chsize
                    kks[0].start = tex.end
                    if kks[0].size() == 0:
                        kks.pop(0)
                    nnn.delete(tex)
                    node.uptasks.append(tex)
                    #for nnn in nodes:
                        #print(nnn)
                    #print(remains)
                    #print(i, node.nid, tt.size())
                    #input('\n')
                    break
            else:
                print('zzz')
                break


if __name__ == '__main__':
    count = -1
    for upload, download in get_bandwidth(bw_path):
        count += 1
        #if count != 15:
            #continue
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
                    print('???', count)
                    print(ut)
                    exit(-1)
                ts[uut.tid] -= uut.size()
        for tts in ts:
            if tts != 0:
                print('kkk', tts, count)
                for nnn in nodes:
                    print(nnn)
                exit(-1)
