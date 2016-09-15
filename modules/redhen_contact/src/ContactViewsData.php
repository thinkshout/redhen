<?php

namespace Drupal\redhen_contact;

use Drupal\views\EntityViewsData;

/**
 * Provides the views data for the redhen_contact entity type.
 */
class ContactViewsData extends EntityViewsData {

  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['redhen_contact']['full_name'] = array(
      'title' => $this->t('Full name'),
      'help' => $this->t('The full name of the contact.'),
      'argument' => array(
        'id' => 'entity_label',
      ),
    );

    return $data;
  }

}
