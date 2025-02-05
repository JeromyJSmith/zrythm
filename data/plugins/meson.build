# SPDX-FileCopyrightText: © 2022 Alexandros Theodotou <alex@zrythm.org>
# SPDX-License-Identifier: LicenseRef-ZrythmLicense

if not get_option ('bundled_plugins')
  subdir_done ()
endif

plugins = [
  [ 'Compressor',
    'Basic compressor',
    'CompressorPlugin' ],
  [ 'Cubic Distortion',
    'Cubic distortion',
    'DistortionPlugin' ],
  [ 'Flanger',
    'Flanger effect',
    'FlangerPlugin' ],
  [ 'Gate Stereo',
    'Stereo gate',
    'GatePlugin' ],
  [ 'Highpass Filter',
    '2nd-order Butterworth highpass filter',
    'HighpassPlugin' ],
  [ 'Lowpass Filter',
    '2nd-order Butterworth lowpass filter',
    'LowpassPlugin' ],
  [ 'White Noise',
    'White noise generator',
    'GeneratorPlugin' ],
  [ 'Parametric EQ',
    'Parametric equalizer with high/low shelves',
    'ParaEQPlugin' ],
  [ 'Peak Limiter',
    '1176 Peak limiter',
    'LimiterPlugin' ],
  [ 'Phaser',
    'Phaser effect',
    'PhaserPlugin' ],
  [ 'Smooth Delay',
    'Delay plugin',
    'DelayPlugin' ],
  [ 'Triple Synth',
    'Synth with 3 detuned oscillators',
    'InstrumentPlugin' ],
  [ 'Wah4',
    'Wah pedal',
    'FilterPlugin' ],
  [ 'Zita Rev1',
    'Zita reverb algorithm',
    'ReverbPlugin' ],
  ]

gen_lv2_sh = configure_file (
  output: 'gen-lv2.sh',
  input: 'gen-lv2.sh.in',
  configuration: {
    'BASH': bash.full_path (),
    'URI_PREFIX': 'https://lv2.zrythm.org/faust-builtin',
    'LIB_SUFFIX': '@LIB_SUFFIX@',
    },
  )

pl_generate_targets = []

foreach pl : plugins
  pl_name = pl[0]
  pl_underscored_name = pl_name.to_lower ().underscorify ()
  pl_descr = pl[1]
  pl_type = pl[2]

  if faust2lv2.found ()
    pl_dsp = configure_file (
      output: pl_underscored_name + '.dsp',
      input: pl_underscored_name + '.dsp.in',
      configuration: {
        'NAME': pl_name,
        'DESCRIPTION': pl_descr,
        'AUTHOR': 'Zrythm DAW',
        },
      )

    plugin = custom_target (
      pl_underscored_name + '-lv2-gen',
      input: [pl_dsp, 'zrythm-utils.lib'],
      output: pl_underscored_name + '.lv2.gen',
      command: [
        gen_lv2_sh, faust2lv2.full_path (),
        '@INPUT0@', '@PRIVATE_DIR@', '@OUTPUT@',
        pl_type, '@INPUT1@',
        meson.current_source_dir () / 'generated'],
      install: false)

    pl_generate_targets += plugin
  endif

  pl_generated_dir = 'generated' / pl_underscored_name + '.lv2'
  pl_install_dir = zrythm_libdir / 'lv2' / pl_underscored_name + '.lv2'

  pl_cpp = pl_generated_dir / pl_underscored_name + '.cpp'
  if fs.is_file (pl_cpp)
    pl_cpp_args = [ '-std=c++11', '-O3', ]
    pl_link_args = []
    if is_gcc and os_gnu and get_option ('build_plugins_with_static_libs')
      # x42: plugins should be statically linked against
      # libstdc++, but not the host. The host should uses
      # system-wide libstdc++ or libc++
      # one exception: windows. you may have to ship
      # libstdc++.dll there
      pl_link_args += [ '-static-libgcc', '-static-libstdc++' ]
    endif
    shared_module (
      pl_underscored_name,
      cpp_args: pl_cpp_args,
      link_args: pl_link_args,
      sources: pl_cpp,
      name_prefix: '',
      gnu_symbol_visibility: 'hidden',
      dependencies: [
        dependency ('boost'),
        lv2_dep,
        ],
      override_options: [
        'b_sanitize=none',
        ],
      install_dir: pl_install_dir,
      install: true)

    pl_manifest_ttl = configure_file (
      output: pl_underscored_name + '-manifest.ttl',
      input: pl_generated_dir / 'manifest.ttl',
      configuration: { 'LIB_SUFFIX': lib_suffix })
    install_data (
      pl_manifest_ttl,
      rename: 'manifest.ttl',
      install_dir: pl_install_dir)

    pl_ttl = configure_file (
      output: pl_underscored_name + '.ttl',
      input: pl_generated_dir / pl_underscored_name + '.ttl',
      configuration: { 'LIB_SUFFIX': lib_suffix })
    install_data (
      pl_ttl, install_dir: pl_install_dir)
  else
    warning (
      pl_cpp + ' not found. Run `ninja -C build gen-faust-plugins` to generate it')
  endif
endforeach

if faust2lv2.found ()
  run_target (
    'gen-faust-plugins',
    command: [
      'echo', 'Generated faust plugins', ],
    depends: pl_generate_targets)
endif
