dst_csv=timing_est.csv
echo "version,Interval" >> "${dst_csv}"


mac_unroll_list="1 2 4 8 16 32"
z_cyclic_list="1 2 4 8 16 32"
shift_unroll_list="1 2 4 8 16 32"

for mac_unroll in ${mac_unroll_list}
do 
	for z_cyclic in ${z_cyclic_list}
	do
		for shift_unroll in ${shift_unroll_list}
		do
			version="${mac_unroll}_${z_cyclic}_${shift_unroll}"
			path_rpt="proj_Top/solution-${version}/syn/report/csynth.rpt"
			interval_value=$(grep "+ Top" ${path_rpt} | head -1 | awk '{print $9}')
			#vitis_hls FFilter.tcl ${mac_unroll} ${z_cyclic} ${shift_unroll}
			echo "${version},${interval_value}" >> "${dst_csv}"
		done
	done
done
