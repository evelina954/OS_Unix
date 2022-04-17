with open('fileA','w+') as file:
	file.write('\x00'*(4*1024*1024+1))
	file.seek(0)
	file.write('\x01')
	file.seek(10000)
	file.write('\x01')
	file.seek(4096*1024)
	file.write('\x01')
	file.flush()
