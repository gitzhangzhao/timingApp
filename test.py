from pygments.styles import *
for style in get_all_styles():
    command = 'dashboard -style syntax_highlighting {!r}'.format(style)
    gdb.execute(command)
    print(command)
    if input('Use this style? (y/N) ') == 'y':
        break
end
