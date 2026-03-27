#!/bin/bash

exe="./a.out"
root="CSE340S26_P1_Provided_Sample_Tests"

pass=0
fail=0

while IFS= read -r -d '' testfile; do
  exp="${testfile}.expected"
  if [[ -f "$exp" ]]; then
    out="$(mktemp)"
    "$exe" < "$testfile" > "$out"

    if diff -u "$exp" "$out" >/dev/null; then
      echo "PASS: $testfile"
      pass=$((pass+1))
    else
      echo "FAIL: $testfile"
      diff -u "$exp" "$out" | head -n 120
      echo "----"
      fail=$((fail+1))
    fi

    rm -f "$out"
  fi
done < <(find "$root" -type f -name '*.txt' ! -name '*.expected' -print0)

echo
echo "Passed: $pass"
echo "Failed: $fail"

exit 0
