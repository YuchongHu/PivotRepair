n , k = 9, 6
bw_path = 'nn.txt'
fail_id = 0
max_bw = 1000
task_num = 3

def get_bandwidth(path):
    data = [line.strip() for line in open(path)]
    data = [[int(b) for b in line.split(' ') if b != ''] for line in data]
    return data[3::3], data[4::3]

def get_capacity(upload, download, kk, rid):
    #fix the bandwidth
    for i, u in enumerate(upload):
        if u <= 0 and i != rid:
            download[i] = 0
    #get upload limit
    exceeded = 0
    while (sum(upload) + exceeded < max(upload) * (k - exceeded)):
        for i in range(len(upload)):
            if (sum(upload) + exceeded < upload[i] * (k - exceeded)):
                upload[i] = -1
                exceeded += 1
    upload_limit = min((sum(upload) + exceeded) // (k - exceeded), max_bw)
    upload = [upload_limit if b == -1 else b for b in upload]
    #get download limit
    download_limit = min(sum(download) // k, max_bw)
    upload = [min(download_limit, b) for b in upload]
    return upload, download, min(upload_limit, download_limit, download[rid])

def get_qualified(upload, download, nn, kk, rid):
    fixed_upload = [0] + upload[1:nn]
    fixed_download = [max_bw] + download[1:nn]
    fresult = get_capacity(fixed_upload, fixed_download, k, rid)
    base = fresult[2] // task_num + 1
    while base > 0:
        result = get_capacity([b // base for b in fresult[0]],
                              [b // base for b in fresult[1]], k, rid)
        if result[2] == 0:
            break
        elif result[2] == task_num:
            return result, base
        base -= 1
    return fresult, base

def get_path(upload, download, tn, kk):
    nodes = [(i, upload[i], download[i]) for i in range(len(upload))]
    up_tasks = [[] for _ in range(len(upload))]
    down_tasks = [{tid: 0 for tid in range(tn)} for _ in range(len(upload))]
    task_ups = [[0, i] for i in range(tn)]
    task_downs = [[0, i] for i in range(tn)]

    nodes.sort(key=lambda x: (x[2] - x[1], x[2]))
    up_tasks[nodes[-1][0]] = [i for i in range(tn)]
    while len(nodes) > 0:
        if sum([task_up[0] for task_up in task_ups]) == tn * kk:
            break
        node = nodes.pop()
        task_ups.sort(key=lambda x: (x[0], sum([y[0] for y in task_downs \
                                                if y[1] == x[1]]), x[1]))
        for i in range(node[1]):
            if task_ups[i][0] >= kk:
                break
            up_tasks[node[0]].append(task_ups[i][1])
            task_ups[i][0] += 1
        for i in range(node[2]):
            task_downs.sort()
            for task_down in task_downs:
                if task_down[0] < kk and task_down[1] in up_tasks[node[0]]:
                    down_tasks[node[0]][task_down[1]] += 1
                    task_down[0] += 1
                    break
            else:
                break
        d = node[2] - sum(down_tasks[node[0]].values())
        if d > 0:
            nodes.insert(0, (node[0], node[1] - len(up_tasks[node[0]]), d))
    nodes.sort(key=lambda x: (x[2] - x[1], x[2]))
    while sum([task_down[0] for task_down in task_downs]) < tn * kk:
        if len(nodes) == 0:
            break
        node = nodes.pop()
        up = node[1]
        down = node[2]
        while down > 0:
            task_downs.sort()
            if task_downs[0][0] == kk:
                break
            tid = task_downs[0][1]
            for i, up_task in enumerate(up_tasks):
                if i == 0:
                    continue
                if tid in up_task and down_tasks[i][tid] == 0:
                    if up <= 0:
                        for u in up_tasks[node[0]]:
                            if u not in up_task:
                                up_task.append(u)
                                up_tasks[node[0]].remove(u)
                                up += 1
                                break
                        else:
                            continue
                    up_task.remove(tid)
                    up_tasks[node[0]].append(tid)
                    added = min(down, kk - task_downs[0][0])
                    down_tasks[node[0]][tid] += added
                    task_downs[0][0] += added
                    down -= added
                    up -= 1
                    break
            else:
                break
        if down > 0:
            break
    tasks = [[-1] * len(upload) for i in range(tn)]
    for i in range(tn):
        if task_ups[i][0] < kk or task_downs[i][0] < kk:
            return False, tasks
    for i in range(n):
        if len(up_tasks[i]) > upload[i] and i != 0:
            return False, tasks
    for i in range(tn):
        task_nodes = [(j, 0 if upload[j] == 0 else 1, down_tasks[j][i]) \
                for j in range(len(upload)) if up_tasks[j].count(i) > 0]
        task_nodes.sort(key=lambda x: (1 - x[1], x[2]))
        queue = [task_nodes.pop()]
        tasks[i][queue[0][0]] = queue[0][0]
        while len(queue) > 0:
            cur_node = queue.pop(0)
            for _ in range(cur_node[2]):
                new_node = task_nodes.pop()
                tasks[i][new_node[0]] = cur_node[0]
                if new_node[2] > 0:
                    queue.append(new_node)
        for j in range(len(upload)):
            tasks[i][j] += 1
    return True, tasks

if __name__ == '__main__':
    uploads, downloads = get_bandwidth(bw_path)
    task_results = []
    uncount = 0
    for i in range(len(uploads)):
        result, b = get_qualified(uploads[i], downloads[i], n, k, fail_id)
        if result[2] == task_num:
            flag, tasks = get_path(result[0], result[1], result[2], k)
            if flag:
                task_results.append(tasks)
            else:
                print(i, result)
                break
        else:
            uncount += 1
            print(i, result, b)
    else:
        print(len(task_results), uncount)
