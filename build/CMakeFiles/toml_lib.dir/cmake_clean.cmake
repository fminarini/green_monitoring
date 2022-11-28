file(REMOVE_RECURSE
  "libtoml_lib.a"
  "libtoml_lib.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/toml_lib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
