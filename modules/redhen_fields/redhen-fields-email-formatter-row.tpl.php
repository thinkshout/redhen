<?php

/**
 * Print the template for a single row of the redhen email widget formatter.
 */

?>

<div class="<?php print implode(' ', $classes_array); ?>">
  <div class="email-label-section">
    <div class="email-address"><?php print check_plain($item['value']) ?></div>
    <?php print check_plain($item['label']) ?>
  </div>
  <div class="bulk-label"><?php print $bulk ?></div>
  <div class="hold-label"><?php print $hold ?></div>
  <div class="primary-label"><?php print $default ?></div>

</div>
