import re
from pathlib import Path
from shutil import move, rmtree
from subprocess import run
from tempfile import TemporaryDirectory

folder_names = {
    "boost-preprocessor": "preprocessor-boost",
    "combblas": "CombBLAS",
    "google-benchmark": "benchmark",
    "gtest": "googletest",
    "liblzma": "xz",
    "nlohmann-json": "json",
    "sdl2": "SDL2",
    "suitesparse": "SuiteSparse",
    "xmp-toolkit-sdk": "XMP-Toolkit-SDK",
}

subprojects_path = Path(__file__).parent.resolve()
assert subprojects_path.name == "subprojects"
project_path = subprojects_path.parent
assert Path.cwd() == project_path

url = "git@github.com:Fingolfin1196/tlaxcaltin.git"

selection_path = project_path / "subprojects.txt"
selection: set[str] | None
if not selection_path.exists():
    print("No subprojects file exists.")
    selection = None
else:
    with open(selection_path, "r") as f:
        selection = {l.strip() for l in f.readlines()}

with TemporaryDirectory() as tmp_dir:
    tmp_path = Path(tmp_dir).resolve()

    package_cache_path = subprojects_path / "packagecache"
    packagecache_tmp_path = tmp_path / "packagecache"

    has_packagecache = package_cache_path.exists()
    if has_packagecache:
        move(package_cache_path, packagecache_tmp_path)

    rmtree(subprojects_path)
    run(["git", "clone", url, "subprojects"], check=True)
    rmtree(subprojects_path / ".git")
    rmtree(subprojects_path / "private")

    # Remove all subprojects that are not desired
    if selection is not None:
        fnames = {folder_names.get(entry, entry) for entry in selection}

        # Remove wraps and direct subfolders
        for p in subprojects_path.iterdir():
            if p.is_file() and p.suffix == ".wrap":
                if p.stem not in selection:
                    p.unlink()
                    continue
            if p.is_dir() and p.name not in fnames | {"packagefiles"}:
                rmtree(p)

        # Remove package files
        package_files_path = subprojects_path / "packagefiles"
        for p in package_files_path.iterdir():
            if p.name not in selection | {"patch"}:
                rmtree(p)
        if len(list(package_files_path.iterdir())) == 0:
            package_files_path.rmdir()

        # Remove patches
        patch_path = package_files_path / "patch"
        for p in patch_path.iterdir():
            if p.stem not in selection:
                p.unlink()
        if len(list(patch_path.iterdir())) == 0:
            patch_path.rmdir()

        # Clean up gitignore
        gitignore_path = subprojects_path / ".gitignore"
        with open(gitignore_path, "r") as f:
            gitignore = f.readlines()
        new_gitignore = []
        folder_re = re.compile(r"/(.*)-\*/")
        for l in gitignore:
            l = l.strip()
            if (m := folder_re.fullmatch(l)) is not None:
                if m.group(1) in fnames:
                    new_gitignore.append(l)
            else:
                new_gitignore.append(l)
        if new_gitignore[-1] == "":
            new_gitignore.pop()
        with open(gitignore_path, "w") as f:
            f.write("".join(f"{l!s}\n" for l in new_gitignore))

    run(["git", "add", "subprojects"], check=True)

    if has_packagecache:
        move(packagecache_tmp_path, package_cache_path)
