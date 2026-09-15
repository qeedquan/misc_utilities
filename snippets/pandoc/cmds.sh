# Render markdown as html and view it in browser
pandoc -t html file.md | lynx -stdin

# Render markdown as pdf
pandoc -t pdf file.md -o file.pdf

