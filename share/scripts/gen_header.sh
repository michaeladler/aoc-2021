#!/bin/sh
set -eu

source_root=$1

days_raw=$(find "$source_root/include" -type f -name "day*.h" | sed -E -e 's!.*include/(.*)\.h!\1!')

echo "$days_raw" | while read -r day; do
    if [ -n "$day" ]; then
        printf '#include "%s.h"\n' "$day"
    fi
done

echo 'static const std::map<int, advent_t> days {'
echo "$days_raw" | while read -r day; do
    day=${day##include}
    day_numeric=${day##day}
    day_numeric=${day_numeric##0}
    if [ -n "$day" ]; then
        printf "    { %s, advent_t{%s} },\n" "$day_numeric" "$day"
    fi
done
echo '};'
