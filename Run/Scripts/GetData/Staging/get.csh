nsls ZElecs2 > list2
foreach file (`cat list2`)
    echo $CASTOR_HOME/ZElecs2/${file}
    rfcp $CASTOR_HOME/ZElecs2/${file} /tmp/hbakhshi/DiElec/
end
