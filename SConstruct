
env = Environment()

env['QT5_DEBUG'] = 1

# Set new vars
env['QT5DIR'] = '/opt/qt-5.0.2/5.0.2/gcc_64'
env['ENV']['PKG_CONFIG_PATH'] = '/opt/qt-5.0.2/5.0.2/gcc_64/lib/pkgconfig'

# Stage 2: add qt5 tool
env.Tool('qt5')

#env.ParseConfig( 'pkg-config --cflags glib-2.0' )

# Libraries:
includeDirs = ['/usr/local/include/',
                env['QT5DIR'] + '/include']
libDirs = ['/usr/local/lib/',
            env['QT5DIR'] + '/lib']

libraries = ['OpenImageIO', 'GL']
                
env.Append(CPPPATH = includeDirs)
env.Append(LIBPATH = libDirs)
env.Append(LIBS = libraries)

# Compile time flags.
env.Append(CCFLAGS = ['-g','-O3', '-fPIC', '-std=c++11'])

# Linker flags
env.Append(LINKFLAGS = ['-Wall'])

env.EnableQt5Modules(['QtGui', 
                    'QtCore', 
                    'QtNetwork', 
                    'QtWidgets',
                    'QtOpenGL'])

# Build the ui classes
env.Uic5(Glob('*.ui'))

### The program itself:

sourceFiles = [ 'GLImageDisplayWidget.cpp',
                'ImageSequenceProvider.cpp',
                'OrkaApplication.cpp', 
                'OrkaImage.cpp',
                'OrkaMainWindow.cpp',
                'OrkaViewSettings.cpp',
                'orka.cpp'
              ]
              
resourceFiles = ['texture.qrc']

# Run 'scons qttest' to compile.
orka = env.Program(target='orka', source=sourceFiles+resourceFiles)

# The default target when just running 'scons' 
Default(orka)
