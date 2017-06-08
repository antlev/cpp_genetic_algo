/ find /proc/irq -name "smp_affinity" | sed -e "s/^.*$/ echo -n 'echo ' ;cat & ;  echo -n '> & #'/g" | bash | tr '\n' " " | tr '#' '\n' > /tmp/saveirq
