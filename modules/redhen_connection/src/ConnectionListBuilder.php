<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;

/**
 * Defines a class to build a listing of Connection entities.
 *
 * @ingroup redhen_connection
 */
class ConnectionListBuilder extends EntityListBuilder {
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['type'] = $this->t('Type');
    $header['name'] = $this->t('Name');
    $header['status'] = $this->t('Status');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $bundles = \Drupal::service('entity_type.bundle.info')->getBundleInfo($entity->getEntityType()->id());
    /* @var $entity \Drupal\redhen_connection\Entity\Connection */
    $row['type'] = $bundles[$entity->getType()]['label'];
    $row['name'] = $entity->toLink()->toString();
    $row['status'] = $entity->isActive() ? $this->t('Active') : $this->t('Inactive');
    return $row + parent::buildRow($entity);
  }

}
