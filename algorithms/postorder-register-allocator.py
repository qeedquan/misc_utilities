#!/usr/bin/env python3

"""

https://en.wikipedia.org/wiki/Sethi%E2%80%93Ullman_algorithm

This is a simplified variant of the Sethi-Ullman algorithm

Sethi-Ullman has more heuristics to evaluate which subtree to parse based on the register value at a subtree.
This algorithm just allocates registers as needed, leading to worse register allocation.

"""

import re

# --- 1. Abstract Syntax Tree (AST) Nodes ---
class Node:
    pass

class Number(Node):
    def __init__(self, value):
        self.value = value

class Var(Node):
    def __init__(self, name):
        self.name = name

class BinOp(Node):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

# --- 2. Minimal Recursive Descent Parser ---
class Parser:
    def __init__(self, text):
        self.tokens = re.findall(r'\d+|[a-zA-Z_]\w*|[\+\-\*/\(\)]', text)
        self.pos = 0

    def peek(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def consume(self):
        token = self.peek()
        self.pos += 1
        return token

    def parse(self):
        return self.expr()

    def expr(self):
        node = self.term()
        while self.peek() in ('+', '-'):
            op = self.consume()
            node = BinOp(node, op, self.term())
        return node

    def term(self):
        node = self.factor()
        while self.peek() in ('*', '/'):
            op = self.consume()
            node = BinOp(node, op, self.factor())
        return node

    def factor(self):
        token = self.peek()
        if token == '(':
            self.consume()  # '('
            node = self.expr()
            self.consume()  # ')'
            return node
        elif token and token.isdigit():
            return Number(self.consume())
        elif token and token.isidentifier():
            return Var(self.consume())
        raise SyntaxError(f"Unexpected token: {token}")

# --- 3. The Register Machine Compiler ---
class RegisterCompiler:
    def __init__(self):
        self.instructions = []
        self.next_reg = 0

    def allocate_register(self):
        reg = f"R{self.next_reg}"
        self.next_reg += 1
        return reg

    def free_register(self):
        # In a basic compiler, we just step back down the linear allocation path
        self.next_reg -= 1

    def compile(self, node):
        """Recursively compiles AST nodes using post-order traversal."""
        if isinstance(node, Number):
            reg = self.allocate_register()
            self.instructions.append(f"LOAD_IMM {reg}, {node.value}")
            return reg

        elif isinstance(node, Var):
            reg = self.allocate_register()
            self.instructions.append(f"LOAD_VAR {reg}, {node.name}")
            return reg

        elif isinstance(node, BinOp):
            # 1. Compile left child -> returns the register holding left result
            left_reg = self.compile(node.left)
            
            # 2. Compile right child -> returns the register holding right result
            right_reg = self.compile(node.right)

            # Map mathematical operators to machine instructions
            op_map = {'+': 'ADD', '-': 'SUB', '*': 'MUL', '/': 'DIV'}
            instr = op_map[node.op]

            # 3. Emit the instruction combining both registers into the left register
            self.instructions.append(f"{instr} {left_reg}, {left_reg}, {right_reg}")
            
            # Right register is no longer needed; return it to the pool
            self.free_register() 
            return left_reg

# --- Execution ---
expression = "(x + 5) * (y - 2) + (z + 4) + (4 * 6) + (u*v + x + y + 599)"
parser = Parser(expression)
ast = parser.parse()

compiler = RegisterCompiler()
final_res_reg = compiler.compile(ast)

print(f"Compilation output for: {expression}\n")
for line in compiler.instructions:
    print(line)
print(f"\nFinal result resides in: {final_res_reg}")
