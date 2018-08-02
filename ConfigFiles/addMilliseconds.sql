CREATE OR REPLACE FUNCTION addMilliseconds()
	RETURNS VOID
AS
$$
DECLARE
	millis INTEGER := 0;
	lastTime TIMESTAMP := (select date from vehicledata limit 1);
	t_curs CURSOR FOR
		SELECT * from vehicledata;
	t_row vehicledata%rowType;
BEGIN
	FOR t_row IN t_curs LOOP
		IF millis = 1000 OR lastTime < (select t_row.date) THEN
			millis := 0;
		END IF;
		lastTime := (select t_row.date);
		UPDATE vehicledata
		SET date = date + millis * INTERVAL '1ms'
		WHERE CURRENT OF t_curs;
		millis := millis + 50;
	END LOOP;
END;
$$
LANGUAGE plpgsql;
		