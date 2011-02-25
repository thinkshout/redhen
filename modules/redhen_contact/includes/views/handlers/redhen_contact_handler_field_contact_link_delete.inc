<?php

/**
 * Field handler to present a link to delete a contact.
 */
class redhen_contact_handler_field_contact_link_delete extends redhen_contact_handler_field_contact_link {
  function construct() {
    parent::construct();
  }

  function render_link($data, $values) {
    // @TODO ensure user has access to delete this contact..
    $contact_id = $values->{$this->aliases['contact_id']};

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "redhen/contact/$contact_id/delete";
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return $text;
  }
}

