(set-width 2048px)
(set-height 700px)
(set-dpi 240)

(default limit-y (-15 30))
(default limit-x (0.5 12.5))

(layout/add-margins margin 1em)

(plot/add-axes
    position (bottom left)
    axis-x-label-placement (linear-alignat 1 1)
    axis-x-label-format (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    axis-y-label-placement (linear-interval 5 -10 30)
    axis-left-title "Temperature (°C)"
    axis-left-title-rotate -90)

(plot/draw-lines
    data-x (csv "test/testdata/city_temperatures.csv" month)
    data-y (csv "test/testdata/city_temperatures.csv" temp_beijing)
    color #aa4643
    marker-size 4pt)

(plot/draw-lines
    data-x (csv "test/testdata/city_temperatures.csv" month)
    data-y (csv "test/testdata/city_temperatures.csv" temp_moscow)
    color #89a54e
    marker-size 4pt)

(plot/draw-lines
    data-x (csv "test/testdata/city_temperatures.csv" month)
    data-y (csv "test/testdata/city_temperatures.csv" temp_telaviv)
    color #4572a7
    marker-size 4pt)

(figure/draw-legend
    position (bottom center)
    border none
    margin 0.5em
    item-flow on
    item (label "Москва" color #89a54e)
    item (label "תל אביב" color #4572a7)
    item (label "北京市" color #aa4643))
