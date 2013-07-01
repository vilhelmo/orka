
env = Environment()


#env.ParseConfig( 'pkg-config --cflags glib-2.0' )

# Libraries:
env.Append(CPPPATH = ['/usr/local/include/'])
env.Append(LIBPATH = ['/usr/local/lib/'])
env.Append(LIBS = ['OpenImageIO', 'glfw'])

# Compile time flags.
env.Append(CCFLAGS = ['-g','-O3'])

# Linker flags
env.Append(LINKFLAGS = ['-Wall'])

### The program itself:
o = env.Program(target='orka', source=['orka.cpp'])

Default(o)
