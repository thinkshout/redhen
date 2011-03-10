<?php

/**
 * Template for displaying an individual redhen contact.
 */

?>
<div id="redhen-contact-<?php print $contact->contact_id; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="field property-first-name field-label-inline clearfix">
    <div class="field-label"><?php print $first_name_label; ?>&nbsp;</div>
    <div class="field-items">
      <div class="field-item even"><?php print $first_name; ?></div>
    </div>
  </div>

  <div class="field property-last-name field-label-inline clearfix">
    <div class="field-label"><?php print $last_name_label; ?>&nbsp;</div>
    <div class="field-items">
      <div class="field-item even"><?php print $last_name; ?></div>
    </div>
  </div>

  <div class="content"<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>
</div>

