<?php

/**
 * Template for displaying an individual redhen contact.
 */

?>
<div id="redhen-contact-<?php print $contact->contact_id; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <?php print $first_name; ?>
  <?php print $last_name; ?>
  <div class="content"<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>
</div>

