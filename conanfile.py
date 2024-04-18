# SPDX-License-Identifier: Apache-2.0

from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.scm import Git, Version
from conans import ConanFile


class zstdstreamConan(ConanFile):
    name = "zstdstream"
    license = "Apache-2.0"
    url = "https://github.com/shigeno-y/zstdstream.git"
    description = "C++ wrapper library for libgit2"
    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"
    no_copy_source = True

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_samples": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_samples": False,
    }

    def set_version(self):
        git = Git(self, self.recipe_folder)

        version_tag = "0.1.0"
        suffix = "-dev"
        diff = 0
        try:
            version_tag = git.run("describe --tags --abbrev=0")
            diff = int(git.run(f"rev-list --count {version_tag}..HEAD"))
            if diff > 0:
                version_tag = str(Version(version_tag).bump(2))
            else:
                suffix = ""
        except Exception:
            pass

        self.version = version_tag + suffix

    def source(self):
        git = Git(self)
        git.run("init")
        git.run(f'remote add origin "{self.url}"')
        git.run("fetch origin")
        try:
            git.run(f"checkout {self.version}")
        except:
            git.run("checkout FETCH_HEAD")

    def build_requirements(self):
        self.build_requires("cmake/[^3.27]")
        # self.build_requires("ninja/[*]")

    def requirements(self):
        self.requires("zstd/[^1.5]")
        self.requires("poco/[^1.13]")

    def configure(self):
        self.options["poco"].shared = self.options.shared
        self.options["poco"].enable_active_record = False
        self.options["poco"].log_debug = False
        self.options["poco"].with_sql_parser = False
        self.options["poco"].enable_apacheconnector = False
        self.options["poco"].enable_cppparser = False
        self.options["poco"].enable_crypto = False
        self.options["poco"].enable_data = False
        self.options["poco"].enable_data_mysql = False
        self.options["poco"].enable_data_odbc = False
        self.options["poco"].enable_data_postgresql = False
        self.options["poco"].enable_data_sqlite = False
        self.options["poco"].enable_encodings = False
        self.options["poco"].enable_json = False
        self.options["poco"].enable_jwt = False
        self.options["poco"].enable_mongodb = False
        self.options["poco"].enable_net = False
        self.options["poco"].enable_netssl = False
        self.options["poco"].enable_netssl_win = False
        self.options["poco"].enable_pdf = False
        self.options["poco"].enable_pagecompiler = False
        self.options["poco"].enable_pagecompiler_file2page = False
        self.options["poco"].enable_pocodoc = False
        self.options["poco"].enable_redis = False
        self.options["poco"].enable_sevenzip = False
        self.options["poco"].enable_util = False
        self.options["poco"].enable_xml = False
        self.options["poco"].enable_zip = False
        self.options["poco"].enable_activerecord = False
        self.options["poco"].enable_activerecord_compiler = False
        self.options["poco"].enable_prometheus = False

    def generate(self):
        # tc = CMakeToolchain(self, generator="Ninja")
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
        tc.variables["ZSTDSTREAM_BUILDSAMPLES"] = bool(self.options.build_samples)
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["zstdstream"]

    def package_id(self):
        del self.info.options.build_samples
