(set width 900px)
(set height 300px)

(plot
    limit-x (0 7)
    limit-y (0 10000)
    axis-x-label-placement (linear-interval 1 1 6)
    scale-y (log)
    axes (left bottom)
    bars (
      data-x (csv "tests/testdata/histogram.csv" var0)
      data-y (csv "tests/testdata/histogram.csv" var1)
      bar-width (1.8em)
      color #666))
