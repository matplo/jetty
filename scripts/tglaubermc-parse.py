#!/usr/bin/env python

import argparse
import os

class AParser(object):
	def __init__(self, fname, indent_char = ' '):
		self.clines = self.get_lines(fname)
		self.chars_from_lines = self.lines_to_chars(self.clines)
		self.chars = self.read_chars(fname)

		self.indent_char = indent_char

		# 	print self.chars
		# 	print
		# print ''.join(self.chars_from_lines)
		#	print self.chars_from_lines

		self.blocks = self.break_on_indentation(self.clines)

		# for b in self.blocks:
		# 	print '- block:'
		# 	for l in b:
		# 		print l
		# 	print

		self.group_class_implementation_blocks()

		with open('tglaubermc.h', 'w') as fout:
			print '- header blocks...'
			for b in self.class_header_blocks:
				for l in b:
					print >> fout,l,

		with open('tglaubermc.cxx', 'w') as fout:
			print >> fout, '#include <tglaubermc.h>'
			for b in self.class_implementation_blocks:
				print '- CXX block for', self.get_class_name(b[0])
				for l in b:
					print >> fout,l,

	def group_class_implementation_blocks(self):
		self.class_header_blocks = []
		self.class_implementation_blocks = []
		processed_classes = []
		for i1,b1 in enumerate(self.blocks):
			cl1 = self.get_class_name(b1[0])
			if cl1 is None:
				if self.is_otherwise_implementation(b1):
					self.class_implementation_blocks.append(b1)
				else:
					self.class_header_blocks.append(b1)
				continue
			if cl1 in processed_classes:
				continue
			else:
				class_block = b1
				processed_classes.append(cl1)
			for i2 in range(i1 + 1, len(self.blocks)):
				b2 = self.blocks[i2]
				cl2 = self.get_class_name(b2[0])
				if cl2 is None:
					continue
				if cl1 == cl2:
					for l in b2:
						class_block.append(l)
			self.class_implementation_blocks.append(class_block)

	def is_otherwise_implementation(self, b):
		for l in b:
			if l.find('ClassImp') >= 0:
				return True
		if b[0].find('class ') == 0:
			return False
		for l in b:
			if l[0] == '{':
				return True;
		return False

	def get_class_name(self, l):
		idx = -1
		try:
			idx = l.index('::')
		except:
			return None
		if idx > -1:
			nspace = l.find(' ')
		else:
			nspace = 0
		cl = l[nspace:idx]
		cl = cl.replace('&', '')
		cl = cl.replace('*', '')
		if len(cl) == 0:
			return None
		return cl

	def read_chars(self, fname):
		data = None
		with open(fname, 'r') as f:
			data=f.read().replace('\n', '')
		return data

	def get_lines(self, fname):
		clines = None
		with open(fname) as f:
			clines = f.readlines()
		return clines

	def lines_to_chars(self, clines):
		chars = []
		for l in clines:
			for c in l:
				if len(chars) > 0:
					if c == '\n' and chars[-1] == '\n':
						continue
					if c == ' ' and chars[-1] == ' ':
						continue
					if c == '\t' and chars[-1] == '\t':
						continue
				if c == '\t':
					continue
				chars.append(c)
		return chars

	def break_on_indentation(self, clines):
		blocks = []
		current_block = []
		curly_open = 0
		for n,l in enumerate(clines):
			nline = n + 1
			if l[0] == '\n':
				continue
			if l[0] == self.indent_char or l[0] == '\t' or l[0] == '#':
				current_block.append(l)
			else:
				if l[0] == '}':
					if curly_open == 1:
						current_block.append(l)
						blocks.append(current_block)
						current_block = []
						curly_open = 0
					else:
						print '[e] curly close error at line', nline, l
						print '[e] current block:'
						print current_block
						return []
				else:
					if l[0] == '{':
						if curly_open != 0:
							print '[e] curly open error at line', nline, l
							print '[e] current block:'
							print current_block
							return []
						else:
							curly_open = 1
							current_block.append(l)
					else:
						if len(current_block) > 0:
							blocks.append(current_block)
						current_block = []
						current_block.append(l)
		return blocks

	def next_line(self, clines, n):
		if n + 1 >= len(clines):
			return ''
		return clines[n+1]

def main():
	argparser = argparse.ArgumentParser(description='another starter not much more...', prog=os.path.basename(__file__))
	argparser.add_argument('filename', help="a filename/path")
	args = argparser.parse_args()
	code_parser = AParser(args.filename) #'runglauber_v3.0.C')

if __name__ == '__main__':
	main()
