#!/usr/bin/env python3
import pystache as TPL
import itertools
import glob
import re

from build_layout import *
from pathlib import Path

def build_example(example):
  url = example["file"]
  print("> Building page: /examples/%s" % url)

  tpl = Path("manual/example.tpl.html").read_text()
  path = "/examples/" + url
  env = {
    "example_url": url,
    "example_src": Path(os.path.join("examples", url + ".fvz")).read_text(),
    "title": "Example: %s" % url,
  }

  html = TPL.render(tpl, env)
  write_page(path, html, title=env["title"])
  copy_file(path + ".svg", os.path.join("examples", url + ".svg"))

def build_example_list(examples):
  tpl = """
  <h1 style="margin-bottom: 0;">Examples</h1>

  {{#list}}
    <div style="margin-bottom: 6em; margin-top: -1em;">
      <h2>{{section}}</h2>
      <div class="example_grid">
      {{#files}}
        <div>
          <section class="info_box">
            <a href="/examples/{{file}}"><h2>Example: <em>{{file}}</em></h2></a>
            <div class="example">
              <a href="/examples/{{file}}"><img src="/examples/{{file}}.svg"></a>
            </div>
          </section>
        </div>
      {{/files}}
      </div>
    </div>
  {{/list}}
  """
  env = {
    "list": examples,
  }

  write_page("/examples", TPL.render(tpl, env), title="Example Gallery", article_class="wide")

def main():
  examples = yaml.load(Path("examples/examples.yaml").read_text())
  examples_all = [{ "file": re.sub("^examples\/", "", re.sub("\.fvz$", "", p)) } for p in glob.glob("examples/**/*.fvz")]

  for example in examples_all:
    build_example(example)

  build_example_list(examples)

main()
