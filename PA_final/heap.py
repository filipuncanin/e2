import math

#TODO Heap
def parent(index):
    return (index - 1) // 2

def left(index):
    return 2*index + 1

def right(index):
    return 2*index + 2

def max_heapify(array, index, heap_size):
    l = left(index)
    r = right(index)

    largest = index

    if l < heap_size and array[l] > array[largest]:
        largest = l

    if r < heap_size and array[r] > array[largest]:
        largest = r

    if largest != index:
        array[index], array[largest] = array[largest], array[index]
        max_heapify(array, largest, heap_size)

def build_max_heap(array, heap_size):
    for i in range(heap_size // 2 - 1, -1, -1):
        max_heapify(array, i, heap_size)

def heap_sort(array, heap_size):
    build_max_heap(array, len(array))
    for i in range(0, len(array)):
        array[0], array[heap_size - 1] = array[heap_size - 1], array[0]
        heap_size -= 1
        max_heapify(array, 0, heap_size)

#TODO Priority queue
def heap_maximum(array):
    return array[0]

def heap_extract_max(array, heap_size):
    if(heap_size < 1):
        print("Error")
        return None

    max = array[0]
    array[0] = array[heap_size - 1]
    heap_size -= 1

    max_heapify(array, 0, heap_size)

    return max, heap_size

if __name__ == "__main__":
    lista = [12, 13, 64, 8, 39, 24, 38, 56, 2]

    print("pre  ", lista)

    build_max_heap(lista, len(lista))

    print("posle ", lista)

    lista.append(100)

    print("100 | ", lista)

    build_max_heap(lista, len(lista))

    print("posle ", lista)