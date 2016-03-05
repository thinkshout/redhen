<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\Entity\Contact.
 */

namespace Drupal\redhen_contact\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Contact entities.
 */
class ContactViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['redhen_contact']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Contact ID'),
      'help' => $this->t('The Contact ID.'),
    );

    return $data;
  }

}
